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

        auto pt = board.at(source).type();

        // castling in chess960
        if (board.chess960() && pt == PieceType::KING && board.at(target).type() == PieceType::ROOK &&
            board.at(target).color() == board.sideToMove()) {
            return Move::make<Move::CASTLING>(source, target);
        }

        // convert to king captures rook
        // in chess960 the move should be sent as king captures rook already!
        if (!board.chess960() && pt == PieceType::KING && Square::distance(target, source) == 2) {
            target = Square(target > source ? File::FILE_H : File::FILE_A, source.rank());
            return Move::make<Move::CASTLING>(source, target);
        }

        // en passant
        if (pt == PieceType::PAWN && target == board.enpassantSq()) {
            return Move::make<Move::ENPASSANT>(source, target);
        }

        // promotion
        if (pt == PieceType::PAWN && uci.length() == 5 && Square::back_rank(target, ~board.sideToMove())) {
            auto promotion = PieceType(uci.substr(4, 1));

            if (promotion == PieceType::NONE || promotion == PieceType::KING || promotion == PieceType::PAWN) {
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

    class AmbiguousMoveError : public std::exception {
       public:
        explicit AmbiguousMoveError(const char *message) : msg_(message) {}

        explicit AmbiguousMoveError(const std::string &message) : msg_(message) {}

        virtual ~AmbiguousMoveError() noexcept {}

        virtual const char *what() const noexcept { return msg_.c_str(); }

       protected:
        std::string msg_;
    };

    /**
     * @brief Parse a san string and return the move.
     * This function will throw a SanParseError if the san string is invalid.
     * @param board
     * @param san
     * @return
     */
    [[nodiscard]] static Move parseSan(const Board &board, std::string_view san) noexcept(false) {
        Movelist moves;

        return parseSan(board, san, moves);
    }

    /**
     * @brief Parse a san string and return the move.
     * This function will throw a SanParseError if the san string is invalid.
     * @param board
     * @param san
     * @param moves
     * @return
     */
    [[nodiscard]] static Move parseSan(const Board &board, std::string_view san, Movelist &moves) noexcept(false) {
        if (san.empty()) {
            return Move::NO_MOVE;
        }

        static constexpr auto pt_to_pgt = [](PieceType pt) { return 1 << (pt); };
        const SanMoveInformation info   = parseSanInfo(san);

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

        Move matchingMove = Move::NO_MOVE;
        bool foundMatch   = false;

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
            else if (info.from != Square::NO_SQ) {
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
#ifndef CHESS_NO_EXCEPTIONS
                throw AmbiguousMoveError("Ambiguous san: " + std::string(san) + " in " + board.getFen());
#endif
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

    /**
     * @brief Check if a string is a valid UCI move. Must also have the correct length.
     * @param move
     * @return
     */
    static bool isUciMove(const std::string &move) noexcept {
        bool is_uci = false;

        static constexpr auto is_digit     = [](char c) { return c >= '1' && c <= '8'; };
        static constexpr auto is_file      = [](char c) { return c >= 'a' && c <= 'h'; };
        static constexpr auto is_promotion = [](char c) { return c == 'n' || c == 'b' || c == 'r' || c == 'q'; };

        // assert that the move is in uci format, [abcdefgh][1-8][abcdefgh][1-8][nbrq]
        if (move.size() >= 4) {
            is_uci = is_file(move[0]) && is_digit(move[1]) && is_file(move[2]) && is_digit(move[3]);
        }

        if (move.size() == 5) {
            is_uci = is_uci && is_promotion(move[4]);
        }

        if (move.size() > 5) {
            return false;
        }

        return is_uci;
    }

   private:
    struct SanMoveInformation {
        File from_file = File::NO_FILE;
        Rank from_rank = Rank::NO_RANK;

        PieceType promotion = PieceType::NONE;

        Square from = Square::NO_SQ;
        // a valid move always has a to square
        Square to = Square::NO_SQ;

        // a valid move always has a piece
        PieceType piece = PieceType::NONE;

        bool castling_short = false;
        bool castling_long  = false;

        bool capture = false;
    };

    [[nodiscard]] static SanMoveInformation parseSanInfo(std::string_view san) noexcept(false) {
#ifndef CHESS_NO_EXCEPTIONS
        if (san.length() < 2) {
            throw SanParseError("Failed to parse san. At step 0: " + std::string(san));
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
        if (handleCastling(move, str)) {
            board.makeMove(move);
            if (board.inCheck()) appendCheckSymbol(board, str);
            return;
        }

        const PieceType pt   = board.at(move.from()).type();
        const bool isCapture = board.at(move.to()) != Piece::NONE || move.typeOf() == Move::ENPASSANT;

        assert(pt != PieceType::NONE);

        if (pt != PieceType::PAWN) {
            appendPieceSymbol(pt, str);
        }

        if constexpr (LAN) {
            appendSquare(move.from(), str);
        } else {
            if (pt == PieceType::PAWN) {
                str += isCapture ? static_cast<std::string>(move.from().file()) : "";
            } else {
                resolveAmbiguity(board, move, pt, str);
            }
        }

        if (isCapture) {
            str += 'x';
        }

        appendSquare(move.to(), str);

        if (move.typeOf() == Move::PROMOTION) appendPromotion(move, str);

        board.makeMove(move);

        if (board.inCheck()) appendCheckSymbol(board, str);
    }

    static bool handleCastling(const Move &move, std::string &str) {
        if (move.typeOf() != Move::CASTLING) return false;

        str = (move.to().file() > move.from().file()) ? "O-O" : "O-O-O";
        return true;
    }

    static void appendPieceSymbol(PieceType pieceType, std::string &str) {
        str += std::toupper(static_cast<std::string>(pieceType)[0]);
    }

    static void appendSquare(Square square, std::string &str) {
        str += static_cast<std::string>(square.file());
        str += static_cast<std::string>(square.rank());
    }

    static void appendPromotion(const Move &move, std::string &str) {
        str += '=';
        str += std::toupper(static_cast<std::string>(move.promotionType())[0]);
    }

    static void appendCheckSymbol(Board &board, std::string &str) {
        const auto gameState = board.isGameOver().second;
        str += (gameState == GameResult::LOSE) ? '#' : '+';
    }

    static void resolveAmbiguity(const Board &board, const Move &move, PieceType pieceType, std::string &str) {
        Movelist moves;
        movegen::legalmoves(moves, board, 1 << pieceType);

        bool needFile         = false;
        bool needRank         = false;
        bool hasAmbiguousMove = false;

        for (const auto &m : moves) {
            if (m != move && m.to() == move.to()) {
                hasAmbiguousMove = true;

                /*
                First, if the moving pieces can be distinguished by their originating files, the originating
                file letter of the moving piece is inserted immediately after the moving piece letter.

                Second (when the first step fails), if the moving pieces can be distinguished by their
                originating ranks, the originating rank digit of the moving piece is inserted immediately after
                the moving piece letter.

                Third (when both the first and the second steps fail), the two character square coordinate of
                the originating square of the moving piece is inserted immediately after the moving piece
                letter.
                */

                if (isIdentifiableByType(moves, move, move.from().file())) {
                    needFile = true;
                    break;
                }

                if (isIdentifiableByType(moves, move, move.from().rank())) {
                    needRank = true;
                    break;
                }
            }
        }

        if (needFile) str += static_cast<std::string>(move.from().file());
        if (needRank) str += static_cast<std::string>(move.from().rank());

        // we weren't able to disambiguate the move by either file or rank, so we need to use both
        if (hasAmbiguousMove && !needFile && !needRank) {
            appendSquare(move.from(), str);
        }
    }

    template <typename CoordinateType>
    static bool isIdentifiableByType(const Movelist &moves, const Move move, CoordinateType type) {
        static_assert(std::is_same_v<CoordinateType, File> || std::is_same_v<CoordinateType, Rank>,
                      "CoordinateType must be either File or Rank");

        for (const auto &m : moves) {
            if (m == move || m.to() != move.to()) {
                continue;
            }

            // file
            if constexpr (std::is_same_v<CoordinateType, File>) {
                if (type == m.from().file()) return false;
            }
            // rank
            else {
                if (type == m.from().rank()) return false;
            }
        }

        return true;
    }
};
}  // namespace chess
