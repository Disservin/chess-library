#pragma once

#include <array>
#include <cassert>
#include <cctype>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "attacks_fwd.hpp"
#include "color.hpp"
#include "constants.hpp"
#include "coords.hpp"
#include "move.hpp"
#include "movegen_fwd.hpp"
#include "piece.hpp"
#include "utils.hpp"
#include "zobrist.hpp"

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

// A compact representation of the board in 24 bytes,
// does not include the half-move clock or full move number.
using PackedBoard = std::array<std::uint8_t, 24>;

class Board {
    using U64 = std::uint64_t;

   public:
    class CastlingRights {
       public:
        enum class Side : uint8_t { KING_SIDE, QUEEN_SIDE };

        void setCastlingRight(Color color, Side castle, File rook_file) {
            rooks[color][static_cast<int>(castle)] = rook_file;
        }

        void clear() {
            rooks[0].fill(File::NO_FILE);
            rooks[1].fill(File::NO_FILE);
        }

        int clear(Color color, Side castle) {
            rooks[color][static_cast<int>(castle)] = File::NO_FILE;

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

        void clear(Color color) { rooks[color].fill(File::NO_FILE); }

        bool has(Color color, Side castle) const { return rooks[color][static_cast<int>(castle)] != File::NO_FILE; }

        bool has(Color color) const {
            return rooks[color][static_cast<int>(Side::KING_SIDE)] != File::NO_FILE ||
                   rooks[color][static_cast<int>(Side::QUEEN_SIDE)] != File::NO_FILE;
        }

        File getRookFile(Color color, Side castle) const { return rooks[color][static_cast<int>(castle)]; }

        int hashIndex() const {
            return has(Color::WHITE, Side::KING_SIDE) + 2 * has(Color::WHITE, Side::QUEEN_SIDE) +
                   4 * has(Color::BLACK, Side::KING_SIDE) + 8 * has(Color::BLACK, Side::QUEEN_SIDE);
        }

        bool isEmpty() const { return !has(Color::WHITE) && !has(Color::BLACK); }

        template <typename T>
        static constexpr Side closestSide(T sq, T pred) {
            return sq > pred ? Side::KING_SIDE : Side::QUEEN_SIDE;
        }

       private:
        // [color][side]
        std::array<std::array<File, 2>, 2> rooks;
    };

   private:
    struct State {
        U64 hash;
        CastlingRights castling;
        Square enpassant;
        uint8_t half_moves;
        Piece captured_piece;

        State(const U64 &hash, const CastlingRights &castling, const Square &enpassant, const uint8_t &half_moves,
              const Piece &captured_piece)
            : hash(hash),
              castling(castling),
              enpassant(enpassant),
              half_moves(half_moves),
              captured_piece(captured_piece) {}
    };

   public:
    explicit Board(std::string_view fen = constants::STARTPOS, bool chess960 = false) {
        prev_states_.reserve(256);
        chess960_ = chess960;
        setFenInternal(fen);
    }

    virtual void setFen(std::string_view fen) { setFenInternal(fen); }

    static Board fromFen(std::string_view fen) { return Board(fen); }
    static Board fromEpd(std::string_view epd) {
        Board board;
        board.setEpd(epd);
        return board;
    }

    void setEpd(const std::string_view epd) {
        auto parts = utils::splitString(epd, ' ');

        if (parts.size() < 1) throw std::runtime_error("Invalid EPD");

        int hm = 0;
        int fm = 1;

        static auto parseStringViewToInt = [](std::string_view sv) -> std::optional<int> {
            if (!sv.empty() && sv.back() == ';') sv.remove_suffix(1);
            try {
                size_t pos;
                int value = std::stoi(std::string(sv), &pos);
                if (pos == sv.size()) return value;
            } catch (...) {
            }
            return std::nullopt;
        };

        if (auto it = std::find(parts.begin(), parts.end(), "hmvc"); it != parts.end()) {
            auto num = *(it + 1);

            hm = parseStringViewToInt(num).value_or(0);
        }

        if (auto it = std::find(parts.begin(), parts.end(), "fmvn"); it != parts.end()) {
            auto num = *(it + 1);

            fm = parseStringViewToInt(num).value_or(1);
        }

        auto fen = std::string(parts[0]) + " " + std::string(parts[1]) + " " + std::string(parts[2]) + " " +
                   std::string(parts[3]) + " " + std::to_string(hm) + " " + std::to_string(fm);

        setFen(fen);
    }

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
                    ss += static_cast<std::string>(piece);
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
        ss += (stm_ == Color::WHITE ? 'w' : 'b');

        // Append the appropriate characters to the FEN string to indicate
        // whether castling is allowed for each player
        if (cr_.isEmpty())
            ss += " -";
        else {
            ss += ' ';
            ss += getCastleString();
        }

        // Append information about the en passant square (if any)
        // and the half-move clock and full move number to the FEN string
        if (ep_sq_ == Square::underlying::NO_SQ)
            ss += " -";
        else {
            ss += ' ';
            ss += static_cast<std::string>(ep_sq_);
        }

        if (move_counters) {
            ss += ' ';
            ss += std::to_string(halfMoveClock());
            ss += ' ';
            ss += std::to_string(fullMoveNumber());
        }

        // Return the resulting FEN string
        return ss;
    }

    [[nodiscard]] std::string getEpd() const {
        std::string ss;
        ss.reserve(100);

        ss += getFen(false);
        ss += " hmvc ";
        ss += std::to_string(halfMoveClock()) + ";";
        ss += " fmvn ";
        ss += std::to_string(fullMoveNumber()) + ";";

        return ss;
    }

    template <bool EXACT = false>
    void makeMove(const Move move) {
        const auto capture  = at(move.to()) != Piece::NONE && move.typeOf() != Move::CASTLING;
        const auto captured = at(move.to());
        const auto pt       = at<PieceType>(move.from());

        // Validate side to move
        assert((at(move.from()) < Piece::BLACKPAWN) == (stm_ == Color::WHITE));

        prev_states_.emplace_back(key_, cr_, ep_sq_, hfm_, captured);

        hfm_++;
        plies_++;

        if (ep_sq_ != Square::underlying::NO_SQ) key_ ^= Zobrist::enpassant(ep_sq_.file());
        ep_sq_ = Square::underlying::NO_SQ;

        if (capture) {
            removePiece(captured, move.to());

            hfm_ = 0;
            key_ ^= Zobrist::piece(captured, move.to());

            // remove castling rights if rook is captured
            if (captured.type() == PieceType::ROOK && Rank::back_rank(move.to().rank(), ~stm_)) {
                const auto king_sq = kingSq(~stm_);
                const auto file    = CastlingRights::closestSide(move.to(), king_sq);

                if (cr_.getRookFile(~stm_, file) == move.to().file()) {
                    key_ ^= Zobrist::castlingIndex(cr_.clear(~stm_, file));
                }
            }
        }

        // remove castling rights if king moves
        if (pt == PieceType::KING && cr_.has(stm_)) {
            key_ ^= Zobrist::castling(cr_.hashIndex());
            cr_.clear(stm_);
            key_ ^= Zobrist::castling(cr_.hashIndex());
        } else if (pt == PieceType::ROOK && Square::back_rank(move.from(), stm_)) {
            const auto king_sq = kingSq(stm_);
            const auto file    = CastlingRights::closestSide(move.from(), king_sq);

            // remove castling rights if rook moves from back rank
            if (cr_.getRookFile(stm_, file) == move.from().file()) {
                key_ ^= Zobrist::castlingIndex(cr_.clear(stm_, file));
            }
        } else if (pt == PieceType::PAWN) {
            hfm_ = 0;

            // double push
            if (Square::value_distance(move.to(), move.from()) == 16) {
                // imaginary attacks from the ep square from the pawn which moved
                Bitboard ep_mask = attacks::pawn(stm_, move.to().ep_square());

                // add enpassant hash if enemy pawns are attacking the square
                if (static_cast<bool>(ep_mask & pieces(PieceType::PAWN, ~stm_))) {
                    if constexpr (EXACT) {
                        const auto piece = at(move.from());
                        removePiece(piece, move.from());
                        placePiece(piece, move.to());
                        stm_ = ~stm_;

                        int double_check = 0;

                        Bitboard occ_us  = us(stm_);
                        Bitboard occ_opp = us(~stm_);
                        auto king_sq     = kingSq(stm_);

                        Bitboard checkmask;
                        Bitboard pin_hv;
                        Bitboard pin_d;

                        if (stm_ == Color::WHITE) {
                            checkmask = movegen::checkMask<Color::WHITE>(*this, king_sq, double_check);
                            pin_hv    = movegen::pinMaskRooks<Color::WHITE>(*this, king_sq, occ_opp, occ_us);
                            pin_d     = movegen::pinMaskBishops<Color::WHITE>(*this, king_sq, occ_opp, occ_us);
                        } else {
                            checkmask = movegen::checkMask<Color::BLACK>(*this, king_sq, double_check);
                            pin_hv    = movegen::pinMaskRooks<Color::BLACK>(*this, king_sq, occ_opp, occ_us);
                            pin_d     = movegen::pinMaskBishops<Color::BLACK>(*this, king_sq, occ_opp, occ_us);
                        }

                        const auto pawns    = pieces(PieceType::PAWN, stm_);
                        const auto pawns_lr = pawns & ~pin_hv;
                        const auto ep       = move.to().ep_square();
                        const auto m        = movegen::generateEPMove(*this, checkmask, pin_d, pawns_lr, ep, stm_);
                        bool found          = false;

                        for (const auto &move : m) {
                            if (move != Move::NO_MOVE) {
                                found = true;
                                break;
                            }
                        }

                        // undo
                        stm_ = ~stm_;

                        removePiece(piece, move.to());
                        placePiece(piece, move.from());

                        if (found) {
                            assert(at(move.to().ep_square()) == Piece::NONE);
                            ep_sq_ = move.to().ep_square();
                            key_ ^= Zobrist::enpassant(move.to().ep_square().file());
                        }
                    } else {
                        assert(at(move.to().ep_square()) == Piece::NONE);
                        ep_sq_ = move.to().ep_square();
                        key_ ^= Zobrist::enpassant(move.to().ep_square().file());
                    }
                }
            }
        }

        if (move.typeOf() == Move::CASTLING) {
            assert(at<PieceType>(move.from()) == PieceType::KING);
            assert(at<PieceType>(move.to()) == PieceType::ROOK);

            const bool king_side = move.to() > move.from();
            const auto rookTo    = Square::castling_rook_square(king_side, stm_);
            const auto kingTo    = Square::castling_king_square(king_side, stm_);

            const auto king = at(move.from());
            const auto rook = at(move.to());

            removePiece(king, move.from());
            removePiece(rook, move.to());

            assert(king == Piece(PieceType::KING, stm_));
            assert(rook == Piece(PieceType::ROOK, stm_));

            placePiece(king, kingTo);
            placePiece(rook, rookTo);

            key_ ^= Zobrist::piece(king, move.from()) ^ Zobrist::piece(king, kingTo);
            key_ ^= Zobrist::piece(rook, move.to()) ^ Zobrist::piece(rook, rookTo);
        } else if (move.typeOf() == Move::PROMOTION) {
            const auto piece_pawn = Piece(PieceType::PAWN, stm_);
            const auto piece_prom = Piece(move.promotionType(), stm_);

            removePiece(piece_pawn, move.from());
            placePiece(piece_prom, move.to());

            key_ ^= Zobrist::piece(piece_pawn, move.from()) ^ Zobrist::piece(piece_prom, move.to());
        } else {
            assert(at(move.from()) != Piece::NONE);
            assert(at(move.to()) == Piece::NONE);

            const auto piece = at(move.from());

            removePiece(piece, move.from());
            placePiece(piece, move.to());

            key_ ^= Zobrist::piece(piece, move.from()) ^ Zobrist::piece(piece, move.to());
        }

        if (move.typeOf() == Move::ENPASSANT) {
            assert(at<PieceType>(move.to().ep_square()) == PieceType::PAWN);

            const auto piece = Piece(PieceType::PAWN, ~stm_);

            removePiece(piece, move.to().ep_square());

            key_ ^= Zobrist::piece(piece, move.to().ep_square());
        }

        key_ ^= Zobrist::sideToMove();
        stm_ = ~stm_;
    }

    void unmakeMove(const Move move) {
        const auto prev = prev_states_.back();
        prev_states_.pop_back();

        ep_sq_ = prev.enpassant;
        cr_    = prev.castling;
        hfm_   = prev.half_moves;
        stm_   = ~stm_;
        plies_--;

        if (move.typeOf() == Move::CASTLING) {
            const bool king_side = move.to() > move.from();

            const auto rook_from_sq = Square(king_side ? File::FILE_F : File::FILE_D, move.from().rank());

            const auto king_to_sq = Square(king_side ? File::FILE_G : File::FILE_C, move.from().rank());

            assert(at<PieceType>(rook_from_sq) == PieceType::ROOK);
            assert(at<PieceType>(king_to_sq) == PieceType::KING);

            const auto rook = at(rook_from_sq);
            const auto king = at(king_to_sq);

            removePiece(rook, rook_from_sq);
            removePiece(king, king_to_sq);
            assert(king == Piece(PieceType::KING, stm_));
            assert(rook == Piece(PieceType::ROOK, stm_));

            placePiece(king, move.from());
            placePiece(rook, move.to());

            key_ = prev.hash;

            return;
        } else if (move.typeOf() == Move::PROMOTION) {
            const auto pawn  = Piece(PieceType::PAWN, stm_);
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

            key_ = prev.hash;
            return;
        } else {
            assert(at(move.to()) != Piece::NONE);

            const auto piece = at(move.to());
            assert(at(move.from()) == Piece::NONE);

            removePiece(piece, move.to());
            placePiece(piece, move.from());
        }

        if (move.typeOf() == Move::ENPASSANT) {
            const auto pawn   = Piece(PieceType::PAWN, ~stm_);
            const auto pawnTo = static_cast<Square>(ep_sq_ ^ 8);

            assert(at(pawnTo) == Piece::NONE);
            placePiece(pawn, pawnTo);
        } else if (prev.captured_piece != Piece::NONE) {
            assert(at(move.to()) == Piece::NONE);
            placePiece(prev.captured_piece, move.to());
        }

        key_ = prev.hash;
    }

    /// @brief Make a null move. (Switches the side to move)
    void makeNullMove() {
        prev_states_.emplace_back(key_, cr_, ep_sq_, hfm_, Piece::NONE);

        key_ ^= Zobrist::sideToMove();
        if (ep_sq_ != Square::underlying::NO_SQ) key_ ^= Zobrist::enpassant(ep_sq_.file());
        ep_sq_ = Square::underlying::NO_SQ;

        stm_ = ~stm_;

        plies_++;
    }

    /// @brief Unmake a null move. (Switches the side to move)
    void unmakeNullMove() {
        const auto &prev = prev_states_.back();

        ep_sq_ = prev.enpassant;
        cr_    = prev.castling;
        hfm_   = prev.half_moves;
        key_   = prev.hash;

        plies_--;

        stm_ = ~stm_;

        prev_states_.pop_back();
    }

    /// @brief Get the occupancy bitboard from us.
    /// @param color
    /// @return
    [[nodiscard]] Bitboard us(Color color) const { return occ_bb_[color]; }

    /// @brief Get the occupancy bitboard of the enemy.
    /// @param color
    /// @return
    [[nodiscard]] Bitboard them(Color color) const { return us(~color); }

    /// @brief Get the current occupancy bitboard.
    /// Faster than calling all() or
    /// us(board.sideToMove()) | them(board.sideToMove()).
    /// @return
    [[nodiscard]] Bitboard occ() const { return occ_bb_[0] | occ_bb_[1]; }

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
    [[nodiscard]] Bitboard pieces(PieceType type, Color color) const { return pieces_bb_[type] & occ_bb_[color]; }

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
            return board_[sq.index()].type();
        } else {
            return board_[sq.index()];
        }
    }

    /// @brief Checks if a move is a capture, enpassant moves are also considered captures.
    /// @param move
    /// @return
    bool isCapture(const Move move) const {
        return (at(move.to()) != Piece::NONE && move.typeOf() != Move::CASTLING) || move.typeOf() == Move::ENPASSANT;
    }

    /// @brief Get the current hash key of the board
    /// @return
    [[nodiscard]] U64 hash() const { return key_; }
    [[nodiscard]] Color sideToMove() const { return stm_; }
    [[nodiscard]] Square enpassantSq() const { return ep_sq_; }
    [[nodiscard]] CastlingRights castlingRights() const { return cr_; }
    [[nodiscard]] std::uint32_t halfMoveClock() const { return hfm_; }
    [[nodiscard]] std::uint32_t fullMoveNumber() const { return 1 + plies_ / 2; }

    void set960(bool is960) {
        chess960_ = is960;
        if (!original_fen_.empty()) setFen(original_fen_);
    }

    /// @brief Checks if the current position is a chess960, aka. FRC/DFRC position.
    /// @return
    [[nodiscard]] bool chess960() const { return chess960_; }

    /// @brief Get the castling rights as a string
    /// @return
    [[nodiscard]] std::string getCastleString() const {
        const auto get_file = [this](Color c, CastlingRights::Side side) {
            auto file = static_cast<std::string>(cr_.getRookFile(c, side));
            return c == Color::WHITE ? std::toupper(file[0]) : file[0];
        };

        if (chess960_) {
            std::string ss;

            for (auto color : {Color::WHITE, Color::BLACK})
                for (auto side : {CastlingRights::Side::KING_SIDE, CastlingRights::Side::QUEEN_SIDE})
                    if (cr_.has(color, side)) ss += get_file(color, side);

            return ss;
        }

        std::string ss;

        if (cr_.has(Color::WHITE, CastlingRights::Side::KING_SIDE)) ss += 'K';
        if (cr_.has(Color::WHITE, CastlingRights::Side::QUEEN_SIDE)) ss += 'Q';
        if (cr_.has(Color::BLACK, CastlingRights::Side::KING_SIDE)) ss += 'k';
        if (cr_.has(Color::BLACK, CastlingRights::Side::QUEEN_SIDE)) ss += 'q';

        return ss;
    }

    /// @brief Checks if the current position is a repetition, set this to 1 if
    /// you are writing a chess engine.
    /// @param count
    /// @return
    [[nodiscard]] bool isRepetition(int count = 2) const {
        uint8_t c = 0;

        // We start the loop from the back and go forward in moves, at most to the
        // last move which reset the half-move counter because repetitions cant
        // be across half-moves.

        for (int i = static_cast<int>(prev_states_.size()) - 2;
             i >= 0 && i >= static_cast<int>(prev_states_.size()) - hfm_ - 1; i -= 2) {
            if (prev_states_[i].hash == key_) c++;

            if (c == count) return true;
        }

        return false;
    }

    /// @brief Checks if the current position is a draw by 50 move rule.
    /// Keep in mind that by the rules of chess, if the position has 50 half
    /// moves it's not necessarily a draw, since checkmate has higher priority,
    /// call getHalfMoveDrawType,
    /// to determine whether the position is a draw or checkmate.
    /// @return
    [[nodiscard]] bool isHalfMoveDraw() const { return hfm_ >= 100; }

    /// @brief Only call this function if isHalfMoveDraw() returns true.
    /// @return
    [[nodiscard]] std::pair<GameResultReason, GameResult> getHalfMoveDrawType() const {
        Movelist movelist;
        movegen::legalmoves(movelist, *this);

        if (movelist.empty() && inCheck()) {
            return {GameResultReason::CHECKMATE, GameResult::LOSE};
        }

        return {GameResultReason::FIFTY_MOVE_RULE, GameResult::DRAW};

    /// @brief Checks if the current position is winnable with any set of legal moves from one side.
    /// @return
    [[nodiscard]] bool hasSufficientMatingMaterial(Color color) const {
        const auto count = us(color).count();

        // lone king, win not possible
        if (count == 1) return false;

        // king with bishop/knight, win not possible
        if (count == 2) {
            if (pieces(PieceType::BISHOP, color) || pieces(PieceType::KNIGHT, color)) return false;
        }

        // king with same colored bishops, win not possible
        if (count == 3) {
            auto bishops = pieces(PieceType::BISHOP, color);
            if (bishops.count() == 2) {
                if (Square::same_color(bishops.lsb(), bishops.msb())) return false;
            }
        }

        return true;
    }

    /// @brief Checks if the current position is a draw by insufficient material.
    /// @return
    [[nodiscard]] bool isInsufficientMaterial() const {
        if (!hasSufficientMatingMaterial(Color::WHITE) && !hasSufficientMatingMaterial(Color::BLACK))
            return true;
        return false;
    }

    /// @brief Checks if the game is over. Returns GameResultReason::NONE if
    /// the game is not over. This function calculates all legal moves for the
    /// current position to
    /// check if the game is over. If you are writing you should not use this
    /// function.
    /// @return
    [[nodiscard]] std::pair<GameResultReason, GameResult> isGameOver() const {
        if (isHalfMoveDraw()) {
            return getHalfMoveDrawType();
        }

        if (isInsufficientMaterial()) return {GameResultReason::INSUFFICIENT_MATERIAL, GameResult::DRAW};

        if (isRepetition()) return {GameResultReason::THREEFOLD_REPETITION, GameResult::DRAW};

        Movelist movelist;
        movegen::legalmoves(movelist, *this);

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
        // cheap checks first
        if (attacks::pawn(~color, square) & pieces(PieceType::PAWN, color)) return true;
        if (attacks::knight(square) & pieces(PieceType::KNIGHT, color)) return true;
        if (attacks::king(square) & pieces(PieceType::KING, color)) return true;

        if (attacks::bishop(square, occ()) & (pieces(PieceType::BISHOP, color) | pieces(PieceType::QUEEN, color)))
            return true;

        if (attacks::rook(square, occ()) & (pieces(PieceType::ROOK, color) | pieces(PieceType::QUEEN, color)))
            return true;

        return false;
    }

    /// @brief Checks if the current side to move is in check
    /// @return
    [[nodiscard]] bool inCheck() const { return isAttacked(kingSq(stm_), ~stm_); }

    /// @brief Checks if the given color has at least 1 piece thats not pawn and not king
    /// @return
    [[nodiscard]] bool hasNonPawnMaterial(Color color) const {
        return bool(pieces(PieceType::KNIGHT, color) | pieces(PieceType::BISHOP, color) |
                    pieces(PieceType::ROOK, color) | pieces(PieceType::QUEEN, color));
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
        if (ep_sq_ != Square::underlying::NO_SQ) ep_hash ^= Zobrist::enpassant(ep_sq_.file());

        U64 stm_hash = 0ULL;
        if (stm_ == Color::WHITE) stm_hash ^= Zobrist::sideToMove();

        U64 castling_hash = 0ULL;
        castling_hash ^= Zobrist::castling(cr_.hashIndex());

        return hash_key ^ ep_hash ^ stm_hash ^ castling_hash;
    }

    friend std::ostream &operator<<(std::ostream &os, const Board &board);

    /// @brief Compresses the board into a PackedBoard
    class Compact {
        friend class Board;
        Compact() = default;

       public:
        /// @brief Compresses the board into a PackedBoard
        static PackedBoard encode(const Board &board) { return encodeState(board); }

        static PackedBoard encode(std::string_view fen, bool chess960 = false) { return encodeState(fen, chess960); }

        /// @brief Creates a Board object from a PackedBoard
        /// @param compressed
        /// @param chess960 If the board is a chess960 position, set this to true
        static Board decode(const PackedBoard &compressed, bool chess960 = false) {
            Board board{};
            board.chess960_ = chess960;
            decode(board, compressed);
            return board;
        }

       private:
        /**
         * A compact board representation can be achieved in 24 bytes,
         * we use 8 bytes (64bit) to store the occupancy bitboard,
         * and 16 bytes (128bit) to store the pieces (plus some special information).
         *
         * Each of the 16 bytes can store 2 pieces, since chess only has 12 different pieces,
         * we can represent the pieces from 0 to 11 in 4 bits (a nibble) and use the other 4 bit for
         * the next piece.
         * Since we need to store information about enpassant, castling rights and the side to move,
         * we can use the remaining 4 bits to store this information.
         *
         * However we need to store the information and the piece information together.
         * This means in our case that
         * 12 -> enpassant + a pawn, we can deduce the color of the pawn from the rank of the square
         * 13 -> white rook with castling rights, we later use the file to deduce if it's a short or long castle
         * 14 -> black rook with castling rights, we later use the file to deduce if it's a short or long castle
         * 15 -> black king and black is side to move
         *
         * We will later deduce the square of the pieces from the occupancy bitboard.
         */
        static PackedBoard encodeState(const Board &board) {
            PackedBoard packed{};

            packed[0] = board.occ().getBits() >> 56;
            packed[1] = (board.occ().getBits() >> 48) & 0xFF;
            packed[2] = (board.occ().getBits() >> 40) & 0xFF;
            packed[3] = (board.occ().getBits() >> 32) & 0xFF;
            packed[4] = (board.occ().getBits() >> 24) & 0xFF;
            packed[5] = (board.occ().getBits() >> 16) & 0xFF;
            packed[6] = (board.occ().getBits() >> 8) & 0xFF;
            packed[7] = board.occ().getBits() & 0xFF;

            auto offset = 8 * 2;
            auto occ    = board.occ();

            while (occ) {
                // we now fill the packed array, since our convertedpiece only actually needs 4 bits,
                // we can store 2 pieces in one byte.
                const auto sq      = Square(occ.pop());
                const auto shift   = (offset % 2 == 0 ? 4 : 0);
                const auto meaning = convertMeaning(board.cr_, board.sideToMove(), board.ep_sq_, sq, board.at(sq));
                const auto nibble  = meaning << shift;

                packed[offset / 2] |= nibble;
                offset++;
            }

            return packed;
        }

        static PackedBoard encodeState(std::string_view fen, bool chess960 = false) {
            // fallback to slower method
            if (chess960) {
                Board board = Board(fen, true);
                return encodeState(board);
            }

            PackedBoard packed{};

            while (fen[0] == ' ') fen.remove_prefix(1);

            const auto params     = split_string_view<6>(fen);
            const auto position   = params[0].has_value() ? *params[0] : "";
            const auto move_right = params[1].has_value() ? *params[1] : "w";
            const auto castling   = params[2].has_value() ? *params[2] : "-";
            const auto en_passant = params[3].has_value() ? *params[3] : "-";

            const auto ep  = en_passant == "-" ? Square::underlying::NO_SQ : Square(en_passant);
            const auto stm = (move_right == "w") ? Color::WHITE : Color::BLACK;

            CastlingRights cr;

            for (char i : castling) {
                if (i == '-') break;

                const auto king_side  = CastlingRights::Side::KING_SIDE;
                const auto queen_side = CastlingRights::Side::QUEEN_SIDE;

                if (i == 'K') cr.setCastlingRight(Color::WHITE, king_side, File::FILE_H);
                if (i == 'Q') cr.setCastlingRight(Color::WHITE, queen_side, File::FILE_A);
                if (i == 'k') cr.setCastlingRight(Color::BLACK, king_side, File::FILE_H);
                if (i == 'q') cr.setCastlingRight(Color::BLACK, queen_side, File::FILE_A);

                assert(i == 'K' || i == 'Q' || i == 'k' || i == 'q');

                continue;
            }

            const auto parts = split_string_view<8>(position, '/');

            auto offset = 8 * 2;
            auto square = 0;
            auto occ    = Bitboard(0);

            for (auto i = parts.rbegin(); i != parts.rend(); i++) {
                auto part = *i;

                for (char curr : *part) {
                    if (isdigit(curr)) {
                        square += (curr - '0');
                    } else if (curr == '/') {
                        square++;
                    } else {
                        const auto p     = Piece(std::string_view(&curr, 1));
                        const auto shift = (offset % 2 == 0 ? 4 : 0);

                        packed[offset / 2] |= convertMeaning(cr, stm, ep, Square(square), p) << shift;
                        offset++;

                        occ.set(square);
                        ++square;
                    }
                }
            }

            packed[0] = occ.getBits() >> 56;
            packed[1] = (occ.getBits() >> 48) & 0xFF;
            packed[2] = (occ.getBits() >> 40) & 0xFF;
            packed[3] = (occ.getBits() >> 32) & 0xFF;
            packed[4] = (occ.getBits() >> 24) & 0xFF;
            packed[5] = (occ.getBits() >> 16) & 0xFF;
            packed[6] = (occ.getBits() >> 8) & 0xFF;
            packed[7] = occ.getBits() & 0xFF;

            return packed;
        }

        static void decode(Board &board, const PackedBoard &compressed) {
            Bitboard occupied = 0ull;

            for (int i = 0; i < 8; i++) {
                occupied |= Bitboard(compressed[i]) << (56 - i * 8);
            }

            int offset           = 16;
            int white_castle_idx = 0, black_castle_idx = 0;
            File white_castle[2] = {File::NO_FILE, File::NO_FILE};
            File black_castle[2] = {File::NO_FILE, File::NO_FILE};

            // clear board state

            board.stm_ = Color::WHITE;
            board.occ_bb_.fill(0ULL);
            board.pieces_bb_.fill(0ULL);
            board.board_.fill(Piece::NONE);
            board.cr_.clear();
            board.original_fen_.clear();
            board.prev_states_.clear();
            board.hfm_   = 0;
            board.plies_ = 0;

            // place pieces back on the board
            while (occupied) {
                const auto sq     = Square(occupied.pop());
                const auto nibble = compressed[offset / 2] >> (offset % 2 == 0 ? 4 : 0) & 0b1111;
                const auto piece  = convertPiece(nibble);

                if (piece != Piece::NONE) {
                    board.placePiece(piece, sq);

                    offset++;
                    continue;
                }

                // Piece has a special meaning, interpret it from the raw integer
                // pawn with ep square behind it
                if (nibble == 12) {
                    board.ep_sq_ = sq.ep_square();
                    // depending on the rank this is a white or black pawn
                    auto color = sq.rank() == Rank::RANK_4 ? Color::WHITE : Color::BLACK;
                    board.placePiece(Piece(PieceType::PAWN, color), sq);
                }
                // castling rights for white
                else if (nibble == 13) {
                    assert(white_castle_idx < 2);
                    white_castle[white_castle_idx++] = sq.file();
                    board.placePiece(Piece(PieceType::ROOK, Color::WHITE), sq);
                }
                // castling rights for black
                else if (nibble == 14) {
                    assert(black_castle_idx < 2);
                    black_castle[black_castle_idx++] = sq.file();
                    board.placePiece(Piece(PieceType::ROOK, Color::BLACK), sq);
                }
                // black to move
                else if (nibble == 15) {
                    board.stm_ = Color::BLACK;
                    board.placePiece(Piece(PieceType::KING, Color::BLACK), sq);
                }

                offset++;
            }

            // reapply castling
            for (int i = 0; i < 2; i++) {
                if (white_castle[i] != File::NO_FILE) {
                    const auto king_sq = board.kingSq(Color::WHITE);
                    const auto file    = white_castle[i];
                    const auto side    = CastlingRights::closestSide(file, king_sq.file());

                    board.cr_.setCastlingRight(Color::WHITE, side, file);
                }

                if (black_castle[i] != File::NO_FILE) {
                    const auto king_sq = board.kingSq(Color::BLACK);
                    const auto file    = black_castle[i];
                    const auto side    = CastlingRights::closestSide(file, king_sq.file());

                    board.cr_.setCastlingRight(Color::BLACK, side, file);
                }
            }

            if (board.stm_ == Color::BLACK) {
                board.plies_++;
            }

            board.key_ = board.zobrist();
        }

        // 1:1 mapping of Piece::internal() to the compressed piece
        static std::uint8_t convertPiece(Piece piece) { return int(piece.internal()); }

        // for pieces with a special meaning return Piece::NONE since this is otherwise not used
        static Piece convertPiece(std::uint8_t piece) {
            if (piece >= 12) return Piece::NONE;
            return Piece(Piece::underlying(piece));
        }

        // 12 => theres an ep square behind the pawn, rank will be deduced from the rank
        // 13 => any white rook with castling rights, side will be deduced from the file
        // 14 => any black rook with castling rights, side will be deduced from the file
        // 15 => black king and black is side to move
        static std::uint8_t convertMeaning(const CastlingRights &cr, Color stm, Square ep, Square sq, Piece piece) {
            if (piece.type() == PieceType::PAWN && ep != Square::underlying::NO_SQ) {
                if (Square(static_cast<int>(sq.index()) ^ 8) == ep) return 12;
            }

            if (piece.type() == PieceType::ROOK) {
                if (piece.color() == Color::WHITE && Square::back_rank(sq, Color::WHITE) &&
                    (cr.getRookFile(Color::WHITE, CastlingRights::Side::KING_SIDE) == sq.file() ||
                     cr.getRookFile(Color::WHITE, CastlingRights::Side::QUEEN_SIDE) == sq.file()))
                    return 13;
                if (piece.color() == Color::BLACK && Square::back_rank(sq, Color::BLACK) &&
                    (cr.getRookFile(Color::BLACK, CastlingRights::Side::KING_SIDE) == sq.file() ||
                     cr.getRookFile(Color::BLACK, CastlingRights::Side::QUEEN_SIDE) == sq.file()))
                    return 14;
            }

            if (piece.type() == PieceType::KING && piece.color() == Color::BLACK && stm == Color::BLACK) {
                return 15;
            }

            return convertPiece(piece);
        }
    };

   protected:
    virtual void placePiece(Piece piece, Square sq) {
        assert(board_[sq.index()] == Piece::NONE);

        auto type  = piece.type();
        auto color = piece.color();
        auto index = sq.index();

        assert(type != PieceType::NONE);
        assert(color != Color::NONE);
        assert(index >= 0 && index < 64);

        pieces_bb_[type].set(index);
        occ_bb_[color].set(index);
        board_[index] = piece;
    }

    virtual void removePiece(Piece piece, Square sq) {
        assert(board_[sq.index()] == piece && piece != Piece::NONE);

        auto type  = piece.type();
        auto color = piece.color();
        auto index = sq.index();

        assert(type != PieceType::NONE);
        assert(color != Color::NONE);
        assert(index >= 0 && index < 64);

        pieces_bb_[type].clear(index);
        occ_bb_[color].clear(index);
        board_[index] = Piece::NONE;
    }

    std::vector<State> prev_states_;

    std::array<Bitboard, 6> pieces_bb_ = {};
    std::array<Bitboard, 2> occ_bb_    = {};
    std::array<Piece, 64> board_       = {};

    U64 key_           = 0ULL;
    CastlingRights cr_ = {};
    uint16_t plies_    = 0;
    Color stm_         = Color::WHITE;
    Square ep_sq_      = Square::underlying::NO_SQ;
    uint8_t hfm_       = 0;

    bool chess960_ = false;

   private:
    /// @brief [Internal Usage]
    /// @param fen
    void setFenInternal(std::string_view fen) {
        original_fen_ = fen;

        occ_bb_.fill(0ULL);
        pieces_bb_.fill(0ULL);
        board_.fill(Piece::NONE);

        // find leading whitespaces and remove them
        while (fen[0] == ' ') fen.remove_prefix(1);

        const auto params     = split_string_view<6>(fen);
        const auto position   = params[0].has_value() ? *params[0] : "";
        const auto move_right = params[1].has_value() ? *params[1] : "w";
        const auto castling   = params[2].has_value() ? *params[2] : "-";
        const auto en_passant = params[3].has_value() ? *params[3] : "-";
        const auto half_move  = params[4].has_value() ? *params[4] : "0";
        const auto full_move  = params[5].has_value() ? *params[5] : "1";

        static auto parseStringViewToInt = [](std::string_view sv) -> std::optional<int> {
            if (!sv.empty() && sv.back() == ';') sv.remove_suffix(1);
            try {
                size_t pos;
                int value = std::stoi(std::string(sv), &pos);
                if (pos == sv.size()) return value;
            } catch (...) {
            }
            return std::nullopt;
        };

        // Half move clock
        hfm_ = parseStringViewToInt(half_move).value_or(0);

        // Full move number
        plies_ = parseStringViewToInt(full_move).value_or(1);

        plies_ = plies_ * 2 - 2;
        ep_sq_ = en_passant == "-" ? Square::underlying::NO_SQ : Square(en_passant);
        stm_   = (move_right == "w") ? Color::WHITE : Color::BLACK;
        key_   = 0ULL;
        cr_.clear();
        prev_states_.clear();

        if (stm_ == Color::BLACK) {
            plies_++;
        } else {
            key_ ^= Zobrist::sideToMove();
        }

        if (ep_sq_ != Square::underlying::NO_SQ) key_ ^= Zobrist::enpassant(ep_sq_.file());

        auto square = 56;
        for (char curr : position) {
            if (isdigit(curr)) {
                square += (curr - '0');
            } else if (curr == '/') {
                square -= 16;
            } else {
                auto p = Piece(std::string_view(&curr, 1));
                placePiece(p, square);
                key_ ^= Zobrist::piece(p, Square(square));
                ++square;
            }
        }

        static const auto find_rook = [](const Board &board, CastlingRights::Side side, Color color) {
            const auto king_side = CastlingRights::Side::KING_SIDE;
            const auto king_sq   = board.kingSq(color);
            const auto sq_corner = Square(side == king_side ? Square::underlying::SQ_H1 : Square::underlying::SQ_A1)
                                       .relative_square(color);

            const auto start = side == king_side ? king_sq + 1 : king_sq - 1;

            for (Square sq = start; (side == king_side ? sq <= sq_corner : sq >= sq_corner);
                 (side == king_side ? sq++ : sq--)) {
                if (board.at<PieceType>(sq) == PieceType::ROOK && board.at(sq).color() == color) {
                    return sq.file();
                }
            }

            throw std::runtime_error("Invalid position");
        };

        for (char i : castling) {
            if (i == '-') break;

            const auto king_side  = CastlingRights::Side::KING_SIDE;
            const auto queen_side = CastlingRights::Side::QUEEN_SIDE;

            if (!chess960_) {
                if (i == 'K') cr_.setCastlingRight(Color::WHITE, king_side, File::FILE_H);
                if (i == 'Q') cr_.setCastlingRight(Color::WHITE, queen_side, File::FILE_A);
                if (i == 'k') cr_.setCastlingRight(Color::BLACK, king_side, File::FILE_H);
                if (i == 'q') cr_.setCastlingRight(Color::BLACK, queen_side, File::FILE_A);

                continue;
            }

            // chess960 castling detection

            const auto color   = isupper(i) ? Color::WHITE : Color::BLACK;
            const auto king_sq = kingSq(color);

            // find rook on the right side of the king
            if (i == 'K' || i == 'k') {
                cr_.setCastlingRight(color, king_side, find_rook(*this, king_side, color));
            }
            // find rook on the left side of the king
            else if (i == 'Q' || i == 'q') {
                cr_.setCastlingRight(color, queen_side, find_rook(*this, queen_side, color));
            }
            // correct frc castling encoding
            else {
                const auto file = File(std::string_view(&i, 1));
                const auto side = CastlingRights::closestSide(file, king_sq.file());
                cr_.setCastlingRight(color, side, file);
            }
        }

        key_ ^= Zobrist::castling(cr_.hashIndex());

        assert(key_ == zobrist());
    }

    template <int N>
    std::array<std::optional<std::string_view>, N> static split_string_view(std::string_view fen,
                                                                            char delimiter = ' ') {
        std::array<std::optional<std::string_view>, N> arr = {};

        std::size_t start = 0;
        std::size_t end   = 0;

        for (std::size_t i = 0; i < N; i++) {
            end = fen.find(delimiter, start);
            if (end == std::string::npos) {
                arr[i] = fen.substr(start);
                break;
            }
            arr[i] = fen.substr(start, end - start);
            start  = end + 1;
        }

        return arr;
    }

    // store the original fen string
    // useful when setting up a frc position and the user called set960(true) afterwards
    std::string original_fen_;
};

inline std::ostream &operator<<(std::ostream &os, const Board &b) {
    for (int i = 63; i >= 0; i -= 8) {
        for (int j = 7; j >= 0; j--) {
            os << " " << static_cast<std::string>(b.board_[i - j]);
        }

        os << " \n";
    }

    os << "\n\n";
    os << "Side to move: " << static_cast<int>(b.stm_.internal()) << "\n";
    os << "Castling rights: " << b.getCastleString() << "\n";
    os << "Halfmoves: " << b.halfMoveClock() << "\n";
    os << "Fullmoves: " << b.fullMoveNumber() << "\n";
    os << "EP: " << b.ep_sq_.index() << "\n";
    os << "Hash: " << b.key_ << "\n";

    os << std::endl;

    return os;
}
}  // namespace  chess
