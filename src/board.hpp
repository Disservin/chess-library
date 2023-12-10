#pragma once

#include <cstdint>
#include <array>
#include <string>
#include <string_view>
#include <vector>
#include <cassert>
#include <charconv>

#include "coords.hpp"
#include "piece.hpp"
#include "color.hpp"
#include "constants.hpp"
#include "move.hpp"
#include "movegen_fwd.hpp"
#include "utils.hpp"
#include "zobrist.hpp"
#include "attacks_fwd.hpp"

namespace chess {

enum class GameResult { WIN, LOSE, DRAW, NONE };

enum class GameResultReason {
    CHECKMATE,
    STALEMATE,
    INSUFFICIENT_MATERIAL,
    FIFTY_MOVE_RULE,
    THREEFOLD_REPETITION,
    NONE
};

class Board {
    using U64 = std::uint64_t;

    class CastlingRights {
       public:
        enum class Side : uint8_t { KING_SIDE, QUEEN_SIDE };

        void setCastlingRight(Color color, Side castle, File rook_file) {
            rooks[static_cast<int>(color.internal())][static_cast<int>(castle)] = rook_file;
        }

        void clear() {
            rooks[0].fill(File::NO_FILE);
            rooks[1].fill(File::NO_FILE);
        }

        int clear(Color color, Side castle) {
            rooks[static_cast<int>(color.internal())][static_cast<int>(castle)] = File::NO_FILE;

            switch (castle) {
                case Side::KING_SIDE:
                    return color == Color::WHITE ? 0 : 2;
                case Side::QUEEN_SIDE:
                    return color == Color::WHITE ? 1 : 3;
                default:
                    assert(false);
                    return -1;
            }
        }

        void clear(Color color) { rooks[static_cast<int>(color.internal())].fill(File::NO_FILE); }

        bool has(Color color, Side castle) const {
            return rooks[static_cast<int>(color.internal())][static_cast<int>(castle)] !=
                   File::NO_FILE;
        }

        bool has(Color color) const {
            return rooks[static_cast<int>(color.internal())][static_cast<int>(Side::KING_SIDE)] !=
                       File::NO_FILE ||
                   rooks[static_cast<int>(color.internal())][static_cast<int>(Side::QUEEN_SIDE)] !=
                       File::NO_FILE;
        }

        File getRookFile(Color color, Side castle) const {
            return rooks[static_cast<int>(color.internal())][static_cast<int>(castle)];
        }

        int hashIndex() const {
            return has(Color::WHITE, Side::KING_SIDE) + 2 * has(Color::WHITE, Side::QUEEN_SIDE) +
                   4 * has(Color::BLACK, Side::KING_SIDE) + 8 * has(Color::BLACK, Side::QUEEN_SIDE);
        }

        bool isEmpty() const { return !has(Color::WHITE) && !has(Color::BLACK); }

       private:
        // [color][side]
        std::array<std::array<File, 2>, 2> rooks;
    };

    struct State {
        CastlingRights castling;
        U64 hash;
        Square enpassant;
        uint8_t half_moves;
        Piece captured_piece;

        State(const U64 &hash, const CastlingRights &castling, const Square &enpassant,
              const uint8_t &half_moves, const Piece &captured_piece)
            : castling(castling),
              hash(hash),
              enpassant(enpassant),
              half_moves(half_moves),
              captured_piece(captured_piece) {}
    };

   public:
    explicit Board(std::string_view fen = constants::STARTPOS) { setFenInternal(fen); }
    virtual void setFen(std::string_view fen) { setFenInternal(fen); }

    /// @brief Get the current FEN string.
    /// @return
    [[nodiscard]] std::string getFen(bool move_counters = true) const {
        std::string ss;
        ss.reserve(100);

        // Loop through the ranks of the board in reverse order
        for (int rank = 7; rank >= 0; rank--) {
            std::uint32_t free_space = 0;

            // Loop through the files of the board
            for (int file = 0; file < 8; file++) {
                // Calculate the square index
                const int sq = rank * 8 + file;

                // If there is a piece at the current square
                if (Piece piece = at(Square(sq)); piece != Piece::NONE) {
                    // If there were any empty squares before this piece,
                    // append the number of empty squares to the FEN string
                    if (free_space) {
                        ss += std::to_string(free_space);
                        free_space = 0;
                    }

                    // Append the character representing the piece to the FEN string
                    ss += static_cast<char>(piece);
                } else {
                    // If there is no piece at the current square, increment the
                    // counter for the number of empty squares
                    free_space++;
                }
            }

            // If there are any empty squares at the end of the rank,
            // append the number of empty squares to the FEN string
            if (free_space != 0) {
                ss += std::to_string(free_space);
            }

            // Append a "/" character to the FEN string, unless this is the last rank
            ss += (rank > 0 ? "/" : "");
        }

        // Append " w " or " b " to the FEN string, depending on which player's turn it is
        ss += ' ';
        ss += (side_to_move_ == Color::WHITE ? 'w' : 'b');
        ss += ' ';

        // Append the appropriate characters to the FEN string to indicate
        // whether castling is allowed for each player
        ss += getCastleString();
        if (castling_rights_.isEmpty()) ss += '-';

        // Append information about the en passant square (if any)
        // and the half-move clock and full move number to the FEN string
        if (enpassant_sq_ == Square::underlying::NO_SQ)
            ss += " - ";
        else {
            ss += ' ';
            ss += static_cast<std::string>(enpassant_sq_);
            ss += ' ';
        }

        if (move_counters) {
            ss += std::to_string(halfMoveClock());
            ss += ' ';
            ss += std::to_string(fullMoveNumber());
        }

        // Return the resulting FEN string
        return ss;
    }

    void makeMove(const Move &move) {
        // Validate side to move
        assert((at(move.from()) < Piece::BLACK_PAWN) == (side_to_move_ == Color::WHITE));
        const auto capture  = at(move.to()) != Piece::NONE && move.typeOf() != Move::CASTLING;
        const auto captured = at(move.to());
        const auto pt       = at<PieceType>(move.from());

        prev_states_.emplace_back(hash_key_, castling_rights_, enpassant_sq_, half_moves_,
                                  captured);

        half_moves_++;
        plies_played_++;

        if (enpassant_sq_ != Square::underlying::NO_SQ)
            hash_key_ ^= Zobrist::enpassant(enpassant_sq_.file());
        enpassant_sq_ = Square::underlying::NO_SQ;

        if (capture) {
            half_moves_ = 0;

            hash_key_ ^= Zobrist::piece(captured, move.to());
            removePiece(captured, move.to());

            const auto rank = move.to().rank();

            if (captured.type() == PieceType::ROOK &&
                ((rank == Rank::RANK_1 && side_to_move_ == Color::BLACK) ||
                 (rank == Rank::RANK_8 && side_to_move_ == Color::WHITE))) {
                const auto king_sq = kingSq(~side_to_move_);
                const auto file    = move.to() > king_sq ? CastlingRights::Side::KING_SIDE
                                                         : CastlingRights::Side::QUEEN_SIDE;

                if (castling_rights_.getRookFile(~side_to_move_, file) == move.to().file()) {
                    const auto idx = castling_rights_.clear(~side_to_move_, file);
                    hash_key_ ^= Zobrist::castlingIndex(idx);
                }
            }
        }

        if (pt == PieceType::KING && castling_rights_.has(side_to_move_)) {
            hash_key_ ^= Zobrist::castling(castling_rights_.hashIndex());

            castling_rights_.clear(side_to_move_);

            hash_key_ ^= Zobrist::castling(castling_rights_.hashIndex());

        } else if (pt == PieceType::ROOK && utils::ourBackRank(move.from(), side_to_move_)) {
            const auto king_sq = kingSq(side_to_move_);
            const auto file    = move.from() > king_sq ? CastlingRights::Side::KING_SIDE
                                                       : CastlingRights::Side::QUEEN_SIDE;

            if (castling_rights_.getRookFile(side_to_move_, file) == move.from().file()) {
                const auto idx = castling_rights_.clear(side_to_move_, file);
                hash_key_ ^= Zobrist::castlingIndex(idx);
            }
        } else if (pt == PieceType::PAWN) {
            half_moves_ = 0;

            const auto possible_ep = static_cast<Square>(move.to() ^ 8);
            if (std::abs(int(move.to().internal()) - int(move.from().internal())) == 16) {
                U64 ep_mask = attacks::pawn(side_to_move_, possible_ep);

                if (ep_mask & pieces(PieceType::PAWN, ~side_to_move_)) {
                    enpassant_sq_ = possible_ep;

                    hash_key_ ^= Zobrist::enpassant(enpassant_sq_.file());
                    assert(at(enpassant_sq_) == Piece::NONE);
                }
            }
        }

        if (move.typeOf() == Move::CASTLING) {
            assert(at<PieceType>(move.from()) == PieceType::KING);
            assert(at<PieceType>(move.to()) == PieceType::ROOK);

            bool king_side = move.to() > move.from();
            auto rookTo    = utils::relativeSquare(
                side_to_move_, king_side ? Square::underlying::SQ_F1 : Square::underlying::SQ_D1);
            auto kingTo = utils::relativeSquare(
                side_to_move_, king_side ? Square::underlying::SQ_G1 : Square::underlying::SQ_C1);

            const auto king = at(move.from());
            const auto rook = at(move.to());

            removePiece(king, move.from());
            removePiece(rook, move.to());

            assert(king == Piece(PieceType::KING, side_to_move_));
            assert(rook == Piece(PieceType::ROOK, side_to_move_));

            placePiece(king, kingTo);
            placePiece(rook, rookTo);

            hash_key_ ^= Zobrist::piece(king, move.from()) ^ Zobrist::piece(king, kingTo);
            hash_key_ ^= Zobrist::piece(rook, move.to()) ^ Zobrist::piece(rook, rookTo);
        } else if (move.typeOf() == Move::PROMOTION) {
            const auto piece_pawn = Piece(PieceType::PAWN, side_to_move_);
            const auto piece_prom = Piece(move.promotionType(), side_to_move_);

            removePiece(piece_pawn, move.from());
            placePiece(piece_prom, move.to());

            hash_key_ ^=
                Zobrist::piece(piece_pawn, move.from()) ^ Zobrist::piece(piece_prom, move.to());
        } else {
            assert(at(move.from()) != Piece::NONE);
            assert(at(move.to()) == Piece::NONE);
            const auto piece = at(move.from());

            removePiece(piece, move.from());
            placePiece(piece, move.to());

            hash_key_ ^= Zobrist::piece(piece, move.from()) ^ Zobrist::piece(piece, move.to());
        }

        if (move.typeOf() == Move::ENPASSANT) {
            assert(at<PieceType>(move.to() ^ 8) == PieceType::PAWN);

            const auto piece = Piece(PieceType::PAWN, ~side_to_move_);

            removePiece(piece, Square(int(move.to().internal()) ^ 8));

            hash_key_ ^= Zobrist::piece(piece, Square(int(move.to().internal()) ^ 8));
        }

        hash_key_ ^= Zobrist::sideToMove();

        side_to_move_ = ~side_to_move_;
    }

    void unmakeMove(const Move &move) {
        const auto prev = prev_states_.back();
        prev_states_.pop_back();

        enpassant_sq_    = prev.enpassant;
        castling_rights_ = prev.castling;
        half_moves_      = prev.half_moves;

        plies_played_--;

        side_to_move_ = ~side_to_move_;

        if (move.typeOf() == Move::CASTLING) {
            const bool king_side = move.to() > move.from();

            const auto rook_from_sq =
                Square(king_side ? File::FILE_F : File::FILE_D, move.from().rank());

            const auto king_to_sq =
                Square(king_side ? File::FILE_G : File::FILE_C, move.from().rank());

            assert(at<PieceType>(rook_from_sq) == PieceType::ROOK);
            assert(at<PieceType>(king_to_sq) == PieceType::KING);

            const auto rook = at(rook_from_sq);
            const auto king = at(king_to_sq);

            removePiece(rook, rook_from_sq);
            removePiece(king, king_to_sq);
            assert(king == Piece(PieceType::KING, side_to_move_));
            assert(rook == Piece(PieceType::ROOK, side_to_move_));

            placePiece(king, move.from());
            placePiece(rook, move.to());

            hash_key_ = prev.hash;

            return;
        } else if (move.typeOf() == Move::PROMOTION) {
            const auto pawn  = Piece(PieceType::PAWN, side_to_move_);
            const auto piece = at(move.to());
            assert(piece.type() == move.promotionType());
            assert(piece.type() != PieceType::PAWN);
            assert(piece.type() != PieceType::KING);
            assert(piece.type() != PieceType::NONE);

            removePiece(piece, move.to());
            placePiece(pawn, move.from());

            if (prev.captured_piece != Piece::NONE) {
                assert(at(move.to()) == Piece::NONE);
                placePiece(prev.captured_piece, move.to());
            }

            hash_key_ = prev.hash;
            return;
        } else {
            assert(at(move.to()) != Piece::NONE);

            const auto piece = at(move.to());
            assert(at(move.from()) == Piece::NONE);

            removePiece(piece, move.to());
            placePiece(piece, move.from());
        }

        if (move.typeOf() == Move::ENPASSANT) {
            const auto pawn   = Piece(PieceType::PAWN, ~side_to_move_);
            const auto pawnTo = static_cast<Square>(enpassant_sq_ ^ 8);

            assert(at(pawnTo) == Piece::NONE);
            placePiece(pawn, pawnTo);
        } else if (prev.captured_piece != Piece::NONE) {
            assert(at(move.to()) == Piece::NONE);
            placePiece(prev.captured_piece, move.to());
        }

        hash_key_ = prev.hash;
    }

    /// @brief Make a null move. (Switches the side to move)
    void makeNullMove() {
        prev_states_.emplace_back(hash_key_, castling_rights_, enpassant_sq_, half_moves_,
                                  Piece::NONE);

        hash_key_ ^= Zobrist::sideToMove();
        if (enpassant_sq_ != Square::underlying::NO_SQ)
            hash_key_ ^= Zobrist::enpassant(enpassant_sq_.file());
        enpassant_sq_ = Square::underlying::NO_SQ;

        side_to_move_ = ~side_to_move_;

        plies_played_++;
    }

    /// @brief Unmake a null move. (Switches the side to move)
    void unmakeNullMove() {
        const auto &prev = prev_states_.back();

        enpassant_sq_    = prev.enpassant;
        castling_rights_ = prev.castling;
        half_moves_      = prev.half_moves;
        hash_key_        = prev.hash;

        plies_played_--;

        side_to_move_ = ~side_to_move_;

        prev_states_.pop_back();
    }

    /// @brief Get the occupancy bitboard from us.
    /// @param color
    /// @return
    [[nodiscard]] Bitboard us(Color color) const {
        return occ_bb_[static_cast<int>(color.internal())];
    }

    /// @brief Get the occupancy bitboard of the enemy.
    /// @param color
    /// @return
    [[nodiscard]] Bitboard them(Color color) const { return us(~color); }

    /// @brief Get the current occupancy bitboard.
    /// Faster than calling all() or
    /// us(board.sideToMove()) | them(board.sideToMove()).
    /// @return
    [[nodiscard]] Bitboard occ() const {
        assert(occ_all_ == all());
        return occ_all_;
    }

    /// @brief recalculate all bitboards
    /// @return
    [[nodiscard]] Bitboard all() const { return us(Color::WHITE) | us(Color::BLACK); }

    /// @brief Returns the square of the king for a certain color
    /// @param color
    /// @return
    [[nodiscard]] Square kingSq(Color color) const {
        assert(pieces(PieceType::KING, color) != Bitboard(0));
        return pieces(PieceType::KING, color).lsb();
    }

    /// @brief Returns all pieces of a certain type and color
    /// @param type
    /// @param color
    /// @return
    [[nodiscard]] Bitboard pieces(PieceType type, Color color) const {
        return pieces_bb_[static_cast<int>(type.internal())] &
               occ_bb_[static_cast<int>(color.internal())];
    }

    /// @brief Returns all pieces of a certain type
    /// @param type
    /// @return
    [[nodiscard]] Bitboard pieces(PieceType type) const {
        return pieces(type, Color::WHITE) | pieces(type, Color::BLACK);
    }

    /// @brief Returns either the piece or the piece type on a square
    /// @tparam T
    /// @param sq
    /// @return
    template <typename T = Piece>
    [[nodiscard]] T at(Square sq) const {
        if constexpr (std::is_same_v<T, PieceType>) {
            return board_[static_cast<int>(sq.internal())].type();
        } else {
            return board_[static_cast<int>(sq.internal())];
        }
    }

    /// @brief Checks if a move is a capture, enpassant moves are also considered captures.
    /// @param move
    /// @return
    bool isCapture(const Move &move) const {
        return (at(move.to()) != Piece::NONE && move.typeOf() != Move::CASTLING) ||
               move.typeOf() == Move::ENPASSANT;
    }

    [[nodiscard]] static Color color(Piece piece) {
        return static_cast<Color>(static_cast<int>(piece.internal()) / 6);
    }

    /// @brief Get the current hash key of the board
    /// @return
    [[nodiscard]] U64 hash() const { return hash_key_; }
    [[nodiscard]] Color sideToMove() const { return side_to_move_; }
    [[nodiscard]] Square enpassantSq() const { return enpassant_sq_; }
    [[nodiscard]] CastlingRights castlingRights() const { return castling_rights_; }
    [[nodiscard]] std::uint32_t halfMoveClock() const { return half_moves_; }
    [[nodiscard]] std::uint32_t fullMoveNumber() const { return 1 + plies_played_ / 2; }

    void set960(bool is960) {
        chess960_ = is960;
        setFen(original_fen_);
    }

    /// @brief Checks if the current position is a chess960, aka. FRC/DFRC position.
    /// @return
    [[nodiscard]] bool chess960() const { return chess960_; }

    /// @brief Get the castling rights as a string
    /// @return
    [[nodiscard]] std::string getCastleString() const {
        std::string ss;

        constexpr auto convert = [](Color c, File file) {
            return c == Color::WHITE ? char(file) + 65 : char(file) + 97;
        };

        const auto get_file = [&](Color c, CastlingRights::Side side) {
            return c == Color::WHITE
                       ? convert(Color::WHITE, castling_rights_.getRookFile(Color::WHITE, side))
                       : convert(Color::WHITE, castling_rights_.getRookFile(Color::BLACK, side));
        };

        if (chess960_) {
            if (castling_rights_.has(Color::WHITE, CastlingRights::Side::KING_SIDE))
                ss += get_file(Color::WHITE, CastlingRights::Side::KING_SIDE);
            if (castling_rights_.has(Color::WHITE, CastlingRights::Side::QUEEN_SIDE))
                ss += get_file(Color::WHITE, CastlingRights::Side::QUEEN_SIDE);
            if (castling_rights_.has(Color::BLACK, CastlingRights::Side::KING_SIDE))
                ss += get_file(Color::BLACK, CastlingRights::Side::KING_SIDE);
            if (castling_rights_.has(Color::BLACK, CastlingRights::Side::QUEEN_SIDE))
                ss += get_file(Color::BLACK, CastlingRights::Side::QUEEN_SIDE);
        } else {
            if (castling_rights_.has(Color::WHITE, CastlingRights::Side::KING_SIDE)) ss += 'K';
            if (castling_rights_.has(Color::WHITE, CastlingRights::Side::QUEEN_SIDE)) ss += 'Q';
            if (castling_rights_.has(Color::BLACK, CastlingRights::Side::KING_SIDE)) ss += 'k';
            if (castling_rights_.has(Color::BLACK, CastlingRights::Side::QUEEN_SIDE)) ss += 'q';
        }

        return ss;
    }

    /// @brief Checks if the current position is a repetition, set this to 1 if you are writing
    /// a chess engine.
    /// @param count
    /// @return
    [[nodiscard]] bool isRepetition(int count = 2) const {
        uint8_t c = 0;

        for (int i = static_cast<int>(prev_states_.size()) - 2;
             i >= 0 && i >= static_cast<int>(prev_states_.size()) - half_moves_ - 1; i -= 2) {
            if (prev_states_[i].hash == hash_key_) c++;

            if (c == count) return true;
        }

        return false;
    }

    /// @brief Checks if the current position is a draw by 50 move rule.
    /// Keep in mind that by the rules of chess, if the position has 50 half moves
    /// it's not necessarily a draw, since checkmate has higher priority, call getHalfMoveDrawType,
    /// to determine whether the position is a draw or checkmate.
    /// @return
    [[nodiscard]] bool isHalfMoveDraw() const { return half_moves_ >= 100; }

    /// @brief Only call this function if isHalfMoveDraw() returns true.
    /// @return
    [[nodiscard]] std::pair<GameResultReason, GameResult> getHalfMoveDrawType() const {
        const Board &board = *this;

        Movelist movelist;
        // movegen::legalmoves<MoveGenType::ALL>(movelist, board);

        if (movelist.empty() && inCheck()) {
            return {GameResultReason::CHECKMATE, GameResult::LOSE};
        }

        return {GameResultReason::FIFTY_MOVE_RULE, GameResult::DRAW};
    }

    /// @brief Checks if the current position is a draw by insufficient material.
    /// @return
    [[nodiscard]] bool isInsufficientMaterial() const {
        const auto count = occ().count();

        if (count == 2) return true;

        if (count == 3) {
            if (pieces(PieceType::BISHOP, Color::WHITE) || pieces(PieceType::BISHOP, Color::BLACK))
                return true;
            if (pieces(PieceType::KNIGHT, Color::WHITE) || pieces(PieceType::KNIGHT, Color::BLACK))
                return true;
        }

        if (count == 4) {
            if (pieces(PieceType::BISHOP, Color::WHITE) &&
                pieces(PieceType::BISHOP, Color::BLACK) &&
                utils::sameColor(pieces(PieceType::BISHOP, Color::WHITE).lsb(),
                                 pieces(PieceType::BISHOP, Color::BLACK).lsb()))
                return true;
        }

        return false;
    }

    /// @brief Checks if the game is over. Returns GameResultReason::NONE if the game is not over.
    /// This function calculates all legal moves for the current position to check if the game is
    /// over. If you are writing you should not use this function.
    /// @return
    [[nodiscard]] std::pair<GameResultReason, GameResult> isGameOver() const {
        if (isHalfMoveDraw()) {
            return getHalfMoveDrawType();
        }

        if (isInsufficientMaterial())
            return {GameResultReason::INSUFFICIENT_MATERIAL, GameResult::DRAW};

        if (isRepetition()) return {GameResultReason::THREEFOLD_REPETITION, GameResult::DRAW};

        const Board &board = *this;

        Movelist movelist;
        // movegen::legalmoves<MoveGenType::ALL>(movelist, board);

        if (movelist.empty()) {
            if (inCheck()) return {GameResultReason::CHECKMATE, GameResult::LOSE};
            return {GameResultReason::STALEMATE, GameResult::DRAW};
        }

        return {GameResultReason::NONE, GameResult::NONE};
    }

    /// @brief Checks if a square is attacked by the given color.
    /// @param square
    /// @param color
    /// @return
    [[nodiscard]] bool isAttacked(Square square, Color color) const {
        if (attacks::pawn(~color, square) & pieces(PieceType::PAWN, color)) return true;
        if (attacks::knight(square) & pieces(PieceType::KNIGHT, color)) return true;
        if (attacks::king(square) & pieces(PieceType::KING, color)) return true;

        if (attacks::bishop(square, occ()) &
            (pieces(PieceType::BISHOP, color) | pieces(PieceType::QUEEN, color)))
            return true;
        if (attacks::rook(square, occ()) &
            (pieces(PieceType::ROOK, color) | pieces(PieceType::QUEEN, color)))
            return true;
        return false;
    }

    /// @brief Checks if the current side to move is in check
    /// @return
    [[nodiscard]] bool inCheck() const { return isAttacked(kingSq(side_to_move_), ~side_to_move_); }

    /// @brief Checks if the given color has at least 1 piece thats not pawn and not king
    /// @return
    [[nodiscard]] bool hasNonPawnMaterial(Color color) const {
        return pieces(PieceType::KNIGHT, color) | pieces(PieceType::BISHOP, color) |
               pieces(PieceType::ROOK, color) | pieces(PieceType::QUEEN, color);
    }

    /// @brief Regenerates the zobrist hash key
    /// @return
    [[nodiscard]] U64 zobrist() const {
        U64 hash_key = 0ULL;

        auto wPieces = us(Color::WHITE);
        auto bPieces = us(Color::BLACK);

        while (wPieces.getBits()) {
            const Square sq = wPieces.pop();
            hash_key ^= Zobrist::piece(at(sq), sq);
        }
        while (bPieces.getBits()) {
            const Square sq = bPieces.pop();
            hash_key ^= Zobrist::piece(at(sq), sq);
        }

        U64 ep_hash = 0ULL;
        if (enpassant_sq_ != Square::underlying::NO_SQ)
            ep_hash ^= Zobrist::enpassant(enpassant_sq_.file());

        U64 side_to_move_hash = 0ULL;
        if (side_to_move_ == Color::WHITE) side_to_move_hash ^= Zobrist::sideToMove();

        // Castle hash

        U64 castling_hash = 0ULL;
        castling_hash ^= Zobrist::castling(castling_rights_.hashIndex());

        return hash_key ^ ep_hash ^ side_to_move_hash ^ castling_hash;
    }

    friend std::ostream &operator<<(std::ostream &os, const Board &board);

   protected:
    virtual void placePiece(Piece piece, Square sq) {
        const auto sq_ = int(sq.internal());
        assert(board_[sq_] == Piece::NONE);

        pieces_bb_[static_cast<int>(piece.type().internal())] |= (1ULL << (sq_));
        occ_bb_[static_cast<int>(piece.color().internal())] |= (1ULL << (sq_));
        occ_all_ |= (1ULL << sq_);

        board_[sq_] = piece;
    }

    virtual void removePiece(Piece piece, Square sq) {
        const auto sq_ = int(sq.internal());
        assert(board_[sq_] == piece && piece != Piece::NONE);

        pieces_bb_[static_cast<int>(piece.type().internal())] &= ~(1ULL << (sq_));
        occ_bb_[static_cast<int>(piece.color().internal())] &= ~(1ULL << (sq_));
        occ_all_ &= ~(1ULL << sq_);

        board_[sq_] = Piece::NONE;
    }

    std::vector<State> prev_states_;

    Bitboard pieces_bb_[6]       = {};
    Bitboard occ_bb_[2]          = {};
    std::array<Piece, 64> board_ = {};

    U64 hash_key_     = 0ULL;
    Bitboard occ_all_ = 0ULL;

    CastlingRights castling_rights_ = {};
    uint16_t plies_played_          = 0;

    Color side_to_move_  = Color::WHITE;
    Square enpassant_sq_ = Square::underlying::NO_SQ;
    uint8_t half_moves_  = 0;

    bool chess960_ = false;

   private:
    /// @brief [Internal Usage]
    /// @param fen
    void setFenInternal(std::string_view fen) {
        original_fen_ = fen;

        std::fill(std::begin(board_), std::end(board_), Piece::NONE);

        // find leading whitespaces and remove them
        while (fen[0] == ' ') fen.remove_prefix(1);

        occ_all_ = 0ULL;

        for (int i = 0; i < 6; i++) {
            pieces_bb_[i] = 0ULL;
        }

        for (const auto c : {0, 1}) {
            occ_bb_[c] = 0ULL;
        }

        const auto params = utils::splitString(fen, ' ');

        const auto position   = params[0];
        const auto move_right = params[1];
        const auto castling   = params[2];
        const auto en_passant = params[3];

        if (params.size() > 4) {
            std::from_chars(params[4].data(), params[4].data() + params[4].size(), half_moves_);
        } else {
            half_moves_ = 0;
        }

        if (params.size() > 5) {
            std::from_chars(params[5].data(), params[5].data() + params[5].size(), plies_played_);
            plies_played_ = plies_played_ * 2 - 2;
        } else {
            plies_played_ = 0;
        }

        side_to_move_ = (move_right == "w") ? Color::WHITE : Color::BLACK;

        if (side_to_move_ == Color::BLACK) {
            plies_played_++;
        }

        auto square = Square(56);
        for (char curr : position) {
            if (Piece(curr) != Piece::NONE) {
                const Piece piece = Piece(curr);
                placePiece(piece, square);
                hash_key_ ^= Zobrist::piece(piece, square);

                square = Square(static_cast<int>(square.internal()) + 1);
            } else if (curr == '/')
                square = Square(static_cast<int>(square.internal()) - 16);
            else if (isdigit(curr)) {
                square = Square(static_cast<int>(square.internal()) + (curr - '0'));
            }
        }

        castling_rights_.clear();

        for (char i : castling) {
            if (i == '-') break;
            if (!chess960_) {
                if (i == 'K')
                    castling_rights_.setCastlingRight(Color::WHITE, CastlingRights::Side::KING_SIDE,
                                                      File::FILE_H);
                if (i == 'Q')
                    castling_rights_.setCastlingRight(
                        Color::WHITE, CastlingRights::Side::QUEEN_SIDE, File::FILE_A);
                if (i == 'k')
                    castling_rights_.setCastlingRight(Color::BLACK, CastlingRights::Side::KING_SIDE,
                                                      File::FILE_H);
                if (i == 'q')
                    castling_rights_.setCastlingRight(
                        Color::BLACK, CastlingRights::Side::QUEEN_SIDE, File::FILE_A);
            } else {
                if (i == 'K' || i == 'k') {
                    // find rook on the right side of the king
                    const auto color   = isupper(i) ? Color::WHITE : Color::BLACK;
                    const auto king_sq = kingSq(color);

                    const auto sq_corner = color == Color::WHITE ? Square::underlying::SQ_H1
                                                                 : Square::underlying::SQ_H8;

                    for (int sq = static_cast<int>(king_sq.internal()) + 1;
                         sq <= static_cast<int>(sq_corner); sq++) {
                        if (at<PieceType>(Square(sq)) == PieceType::NONE) continue;
                        if (at<PieceType>(Square(sq)) == PieceType::ROOK &&
                            int(at(Square(sq)).internal()) / 6 == int(color)) {
                            castling_rights_.setCastlingRight(
                                color, CastlingRights::Side::KING_SIDE, Square(sq).file());
                            break;
                        }
                    }

                } else if (i == 'Q' || i == 'q') {
                    // find rook on the left side of the king
                    const auto color   = isupper(i) ? Color::WHITE : Color::BLACK;
                    const auto king_sq = kingSq(color);

                    const auto sq_corner = color == Color::WHITE ? Square::underlying::SQ_A1
                                                                 : Square::underlying::SQ_A8;

                    for (int sq = int(king_sq.internal()) - 1; sq >= int(sq_corner); sq--) {
                        if (at<PieceType>(Square(sq)) == PieceType::NONE) continue;
                        if (at<PieceType>(Square(sq)) == PieceType::ROOK &&
                            int(at(Square(sq)).internal()) / 6 == int(color)) {
                            castling_rights_.setCastlingRight(
                                color, CastlingRights::Side::QUEEN_SIDE, Square(sq).file());
                            break;
                        }
                    }
                } else {
                    const auto color   = isupper(i) ? Color::WHITE : Color::BLACK;
                    const auto king_sq = kingSq(color);
                    const auto file    = static_cast<File>(tolower(i) - 97);
                    const auto side    = int(file.internal()) > int(king_sq.file().internal())
                                             ? CastlingRights::Side::KING_SIDE
                                             : CastlingRights::Side::QUEEN_SIDE;
                    castling_rights_.setCastlingRight(color, side, file);
                }
            }
        }

        if (en_passant == "-") {
            enpassant_sq_ = Square::underlying::NO_SQ;
        } else {
            const char letter = en_passant[0];
            const int file    = letter - 96;
            const int rank    = en_passant[1] - 48;
            enpassant_sq_     = Square((rank - 1) * 8 + file - 1);
        }

        hash_key_ = zobrist();
        occ_all_  = all();

        prev_states_.clear();
        prev_states_.reserve(150);
    }

    std::string original_fen_;
};
}  // namespace  chess