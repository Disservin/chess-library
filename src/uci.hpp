#pragma once

#include <cassert>
#include <cctype>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

#include "board.hpp"
#include "color.hpp"
#include "move.hpp"
#include "movegen_fwd.hpp"

namespace chess {
class uci {
   public:
    /**
     * @brief Converts an internal move to a UCI string
     * @param move
     * @param chess960
     * @return
     */
    [[nodiscard]] static std::string moveToUci(const Move &move, bool chess960 = false) noexcept(false) {
        // Get the from and to squares
        Square from_sq = move.from();
        Square to_sq   = move.to();

        // If the move is not a chess960 castling move and is a king moving more than one square,
        // update the to square to be the correct square for a regular castling move
        if (!chess960 && move.typeOf() == Move::CASTLING) {
            to_sq = Square(to_sq > from_sq ? File::FILE_G : File::FILE_C, from_sq.rank());
        }

        std::stringstream ss;

        // Add the from and to squares to the string stream
        ss << from_sq;
        ss << to_sq;

        // If the move is a promotion, add the promoted piece to the string stream
        if (move.typeOf() == Move::PROMOTION) {
            ss << static_cast<std::string>(move.promotionType());
        }

        return ss.str();
    }

    /**
     * @brief Converts a UCI string to an internal move.
     * @param board
     * @param uci
     * @return
     */
    [[nodiscard]] static Move uciToMove(const Board &board, const std::string &uci) noexcept(false) {
        if (uci.length() < 4) {
            return Move::NO_MOVE;
        }

        Square source = Square(uci.substr(0, 2));
        Square target = Square(uci.substr(2, 2));

        if (!source.is_valid() || !target.is_valid()) {
            return Move::NO_MOVE;
        }

        PieceType piece = board.at(source).type();

        // castling in chess960
        if (board.chess960() && piece == PieceType::KING && board.at(target).type() == PieceType::ROOK &&
            board.at(target).color() == board.sideToMove()) {
            return Move::make<Move::CASTLING>(source, target);
        }

        // convert to king captures rook
        // in chess960 the move should be sent as king captures rook already!
        if (!board.chess960() && piece == PieceType::KING && Square::distance(target, source) == 2) {
            target = Square(target > source ? File::FILE_H : File::FILE_A, source.rank());
            return Move::make<Move::CASTLING>(source, target);
        }

        // en passant
        if (piece == PieceType::PAWN && target == board.enpassantSq()) {
            return Move::make<Move::ENPASSANT>(source, target);
        }

        // promotion
        if (piece == PieceType::PAWN && uci.length() == 5 && Square::back_rank(target, ~board.sideToMove())) {
            auto promotion = PieceType(uci.substr(4, 1));

            if (promotion != PieceType::QUEEN && promotion != PieceType::ROOK && promotion != PieceType::BISHOP &&
                promotion != PieceType::KNIGHT) {
                return Move::NO_MOVE;
            }

            return Move::make<Move::PROMOTION>(source, target, PieceType(uci.substr(4, 1)));
        }

        switch (uci.length()) {
            case 4:
                return Move::make<Move::NORMAL>(source, target);
            default:
                return Move::NO_MOVE;
        }
    }

    /**
     * @brief Converts a move to a SAN string
     * @param board
     * @param move
     * @return
     */
    [[nodiscard]] static std::string moveToSan(const Board &board, const Move &move) noexcept(false) {
        std::string san;
        moveToRep<false>(board, move, san);
        return san;
    }

    /**
     * @brief Converts a move to a LAN string
     * @param board
     * @param move
     * @return
     */
    [[nodiscard]] static std::string moveToLan(const Board &board, const Move &move) noexcept(false) {
        std::string lan;
        moveToRep<true>(board, move, lan);
        return lan;
    }

    class SanParseError : public std::exception {
       public:
        explicit SanParseError(const char *message) : msg_(message) {}

        explicit SanParseError(const std::string &message) : msg_(message) {}

        virtual ~SanParseError() noexcept {}

        virtual const char *what() const noexcept { return msg_.c_str(); }

       protected:
        std::string msg_;
    };

    /**
     * @brief Parse a san string and return the move.
     * @tparam PEDANTIC
     * @param board
     * @param san
     * @return
     */
    template <bool PEDANTIC = false>
    [[nodiscard]] static Move parseSan(const Board &board, std::string_view san) noexcept(false) {
        Movelist moves;

        return parseSan<PEDANTIC>(board, san, moves);
    }

    template <bool PEDANTIC = false>
    [[nodiscard]] static Move parseSan(const Board &board, std::string_view san, Movelist &moves) noexcept(false) {
        if (san.empty()) {
            return Move::NO_MOVE;
        }

        static constexpr auto pt_to_pgt = [](PieceType pt) { return 1 << (pt); };
        const SanMoveInformation info   = parseSanInfo<PEDANTIC>(san);

        if (info.capture) {
            movegen::legalmoves<movegen::MoveGenType::CAPTURE>(moves, board, pt_to_pgt(info.piece));
        } else {
            movegen::legalmoves<movegen::MoveGenType::QUIET>(moves, board, pt_to_pgt(info.piece));
        }

        if (info.castling_short || info.castling_long) {
            for (const auto &move : moves) {
                if (move.typeOf() == Move::CASTLING) {
                    if ((info.castling_short && move.to() > move.from()) ||
                        (info.castling_long && move.to() < move.from())) {
                        return move;
                    }
                }
            }

#ifndef CHESS_NO_EXCEPTIONS
            throw SanParseError("Failed to parse san. At step 2: " + std::string(san) + " " + board.getFen());
#endif
        }

        Move matchingMove;
        bool foundMatch = false;

        for (const auto &move : moves) {
            // Skip all moves that are not to the correct square
            // or are castling moves
            if (move.to() != info.to || move.typeOf() == Move::CASTLING) {
                continue;
            }

            // Handle promotion moves
            if (info.promotion != PieceType::NONE) {
                if (move.typeOf() != Move::PROMOTION || info.promotion != move.promotionType() ||
                    move.from().file() != info.from_file) {
                    continue;
                }
            }
            // Handle en passant moves
            else if (move.typeOf() == Move::ENPASSANT) {
                if (move.from().file() != info.from_file) {
                    continue;
                }
            }
            // Handle moves with specific from square
            else if (info.from != Square::underlying::NO_SQ) {
                if (move.from() != info.from) {
                    continue;
                }
            }
            // Handle moves with partial from information (rank or file)
            else if (info.from_rank != Rank::NO_RANK || info.from_file != File::NO_FILE) {
                if ((info.from_file != File::NO_FILE && move.from().file() != info.from_file) ||
                    (info.from_rank != Rank::NO_RANK && move.from().rank() != info.from_rank)) {
                    continue;
                }
            }

            // If we get here, the move matches our criteria
            if (foundMatch) {
                throw SanParseError("Ambiguous san: " + std::string(san) + " in " + board.getFen());
            }

            matchingMove = move;
            foundMatch   = true;
        }

        if (!foundMatch) {
#ifndef CHESS_NO_EXCEPTIONS
            throw SanParseError("Failed to parse san. At step 3: " + std::string(san) + " " + board.getFen());
#endif
        }

        return matchingMove;
    }

   private:
    struct SanMoveInformation {
        File from_file = File::NO_FILE;
        Rank from_rank = Rank::NO_RANK;

        PieceType promotion = PieceType::NONE;

        Square from = Square::underlying::NO_SQ;
        Square to   = Square::underlying::NO_SQ;  // a valid move always has a to square

        PieceType piece = PieceType::NONE;  // a valid move always has a piece

        bool castling_short = false;
        bool castling_long  = false;

        bool capture = false;
    };

    template <bool PEDANTIC = false>
    [[nodiscard]] static SanMoveInformation parseSanInfo(std::string_view san) noexcept(false) {
#ifndef CHESS_NO_EXCEPTIONS
        if constexpr (PEDANTIC) {
            if (san.length() < 2) {
                throw SanParseError("Failed to parse san. At step 0: " + std::string(san));
            }
        }
#endif
        constexpr auto parse_castle = [](std::string_view &san, SanMoveInformation &info, char castling_char) {
            info.piece = PieceType::KING;

            san.remove_prefix(3);

            info.castling_short = san.length() == 0 || (san.length() >= 1 && san[0] != '-');
            info.castling_long  = san.length() >= 2 && san[0] == '-' && san[1] == castling_char;

            assert((info.castling_short && !info.castling_long) || (!info.castling_short && info.castling_long) ||
                   (!info.castling_short && !info.castling_long));
        };

        static constexpr auto isRank = [](char c) { return c >= '1' && c <= '8'; };
        static constexpr auto isFile = [](char c) { return c >= 'a' && c <= 'h'; };
        static constexpr auto sw     = [](const char &c) { return std::string_view(&c, 1); };

        SanMoveInformation info;

        // set to 1 to skip piece type offset
        std::size_t index = 1;

        if (san[0] == 'O' || san[0] == '0') {
            parse_castle(san, info, san[0]);
            return info;
        } else if (isFile(san[0])) {
            index--;
            info.piece = PieceType::PAWN;
        } else {
            info.piece = PieceType(san);
        }

        File file_to = File::NO_FILE;
        Rank rank_to = Rank::NO_RANK;

        // check if san starts with a file, if so it will be start file
        if (index < san.size() && isFile(san[index])) {
            info.from_file = File(sw(san[index]));
            index++;
        }

        // check if san starts with a rank, if so it will be start rank
        if (index < san.size() && isRank(san[index])) {
            info.from_rank = Rank(sw(san[index]));
            index++;
        }

        // skip capture sign
        if (index < san.size() && san[index] == 'x') {
            info.capture = true;
            index++;
        }

        // to file
        if (index < san.size() && isFile(san[index])) {
            file_to = File(sw(san[index]));
            index++;
        }

        // to rank
        if (index < san.size() && isRank(san[index])) {
            rank_to = Rank(sw(san[index]));
            index++;
        }

        // promotion
        if (index < san.size() && san[index] == '=') {
            index++;
            info.promotion = PieceType(sw(san[index]));

#ifndef CHESS_NO_EXCEPTIONS
            if (info.promotion == PieceType::KING || info.promotion == PieceType::PAWN ||
                info.promotion == PieceType::NONE)
                throw SanParseError("Failed to parse promotion, during san conversion." + std::string(san));
#endif

            index++;
        }

        // for simple moves like Nf3, e4, etc. all the information is contained
        // in the from file and rank. Thus we need to move it to the to file and rank.
        if (file_to == File::NO_FILE && rank_to == Rank::NO_RANK) {
            file_to = info.from_file;
            rank_to = info.from_rank;

            info.from_file = File::NO_FILE;
            info.from_rank = Rank::NO_RANK;
        }

        // pawns which are not capturing stay on the same file
        if (info.piece == PieceType::PAWN && info.from_file == File::NO_FILE && !info.capture) {
            info.from_file = file_to;
        }

        info.to = Square(file_to, rank_to);

        if (info.from_file != File::NO_FILE && info.from_rank != Rank::NO_RANK) {
            info.from = Square(info.from_file, info.from_rank);
        }

        return info;
    }

    template <bool LAN = false>
    static void moveToRep(Board board, const Move &move, std::string &str) {
        if (move.typeOf() == Move::CASTLING) {
            str = move.to() > move.from() ? "O-O" : "O-O-O";
            return;
        }

        const PieceType pt = board.at(move.from()).type();

        assert(pt != PieceType::NONE);

        if (pt != PieceType::PAWN) {
            str += std::toupper(static_cast<std::string>(pt)[0]);
        }

        if constexpr (LAN) {
            str += static_cast<std::string>(move.from().file());
            str += static_cast<std::string>(move.from().rank());
        } else if (pt != PieceType::PAWN) {
            Movelist moves;
            movegen::legalmoves(moves, board, 1 << pt);

            bool needFile = false;
            bool needRank = false;

            for (const auto &m : moves) {
                if (m != move && m.to() == move.to()) {
                    // same file but different rank, we need rank
                    if (m.from().file() == move.from().file()) needRank = true;

                    // same rank but different file, we need file
                    if (m.from().rank() == move.from().rank()) needFile = true;
                }
            }

            if (needFile) str += static_cast<std::string>(move.from().file());
            if (needRank) str += static_cast<std::string>(move.from().rank());
        }

        if (board.at(move.to()) != Piece::NONE || move.typeOf() == Move::ENPASSANT) {
            if (pt == PieceType::PAWN) {
                str += static_cast<std::string>(move.from().file());
            }

            str += 'x';
        }

        str += static_cast<std::string>(move.to().file());
        str += static_cast<std::string>(move.to().rank());

        if (move.typeOf() == Move::PROMOTION) {
            const auto promotion_pt = static_cast<std::string>(move.promotionType());

            str += '=';
            str += std::toupper(promotion_pt[0]);
        }

        board.makeMove(move);

        if (!board.inCheck()) {
            return;
        }

        if (board.isGameOver().second == GameResult::LOSE) {
            str += '#';
        } else {
            str += '+';
        }
    }
};
}  // namespace chess
