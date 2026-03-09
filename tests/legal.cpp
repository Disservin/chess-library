#include <iostream>
#include <vector>

#include "../src/include.hpp"
#include "doctest/doctest.hpp"

using namespace chess;
using std::cout;
using std::flush;
using std::string;
using std::vector;

class LegalChecker {
   private:
    vector<Move> allmoves_;
    bool chess960_;

   public:
    LegalChecker(bool chess960) : chess960_(chess960) {}

    void init_allmoves() {
        // normal moves
        for (Square from = Square(Square::SQ_A1); from <= Square(Square::SQ_H8); from++) {
            Bitboard to_squares = attacks::knight(from) | attacks::bishop(from, 0) | attacks::rook(from, 0);

            while (to_squares) {
                const auto to = static_cast<Square>(to_squares.pop());
                allmoves_.emplace_back(Move::make<Move::NORMAL>(from, to));
            }
        }

        // enpassants
        for (const auto color : {Color::WHITE, Color::BLACK}) {
            const auto from_rank = Rank::rank(Rank::RANK_5, color);

            for (File file = File(File::FILE_A); file <= File(File::FILE_H); file += 1) {
                const auto from = Square(file, from_rank);
                auto to_squares = attacks::pawn(color, from);

                while (to_squares) {
                    const auto to = static_cast<Square>(to_squares.pop());
                    allmoves_.emplace_back(Move::make<Move::ENPASSANT>(from, to));
                }
            }
        }

        // promotions
        for (const auto color : {Color::WHITE, Color::BLACK}) {
            const auto from_rank = Rank::rank(Rank::RANK_7, color);
            const auto to_rank   = Rank::rank(Rank::RANK_8, color);

            for (File file = File(File::FILE_A); file <= File(File::FILE_H); file += 1) {
                const auto from = Square(file, from_rank);
                auto to_squares = Bitboard::fromSquare(Square(file, to_rank)) | attacks::pawn(color, from);

                while (to_squares) {
                    const auto to = static_cast<Square>(to_squares.pop());
                    allmoves_.emplace_back(Move::make<Move::PROMOTION>(from, to, PieceType::KNIGHT));
                    allmoves_.emplace_back(Move::make<Move::PROMOTION>(from, to, PieceType::BISHOP));
                    allmoves_.emplace_back(Move::make<Move::PROMOTION>(from, to, PieceType::ROOK));
                    allmoves_.emplace_back(Move::make<Move::PROMOTION>(from, to, PieceType::QUEEN));
                }
            }
        }

        // castling
        if (!chess960_) {
            allmoves_.emplace_back(Move::make<Move::CASTLING>(Square::SQ_E1, Square::SQ_H1));
            allmoves_.emplace_back(Move::make<Move::CASTLING>(Square::SQ_E1, Square::SQ_A1));
            allmoves_.emplace_back(Move::make<Move::CASTLING>(Square::SQ_E8, Square::SQ_H8));
            allmoves_.emplace_back(Move::make<Move::CASTLING>(Square::SQ_E8, Square::SQ_A8));
        } else {
            for (File king_file = File(File::FILE_A); king_file <= File(File::FILE_H); king_file += 1) {
                for (File rook_file = File(File::FILE_A); rook_file <= File(File::FILE_H); rook_file += 1) {
                    if (king_file == rook_file) continue;
                    allmoves_.emplace_back(
                        Move::make<Move::CASTLING>(Square(king_file, Rank::RANK_1), Square(rook_file, Rank::RANK_1)));
                    allmoves_.emplace_back(
                        Move::make<Move::CASTLING>(Square(king_file, Rank::RANK_8), Square(rook_file, Rank::RANK_8)));
                }
            }
        }
    }

    void check(const Board& board, int depth) {
        if (depth == 0) return;

        Movelist legalmoves;
        movegen::legalmoves(legalmoves, board);

        for (const auto& move : allmoves_) {
            const bool is_legal    = std::find(legalmoves.begin(), legalmoves.end(), move) != legalmoves.end();
            const bool check_legal = board.isLegal(move);

            if (check_legal != is_legal) {
                string move_type;
                if (move.typeOf() == Move::ENPASSANT)
                    move_type = " ep";
                else if (move.typeOf() == Move::CASTLING)
                    move_type = " cs";
                else if (move.typeOf() == Move::PROMOTION)
                    move_type = " pr";
                else
                    move_type = "";

                string movestr = static_cast<std::string>(move.from()) + static_cast<std::string>(move.to());

                cout << "is_legal failed for position " << board.getFen() << " move " << movestr << move_type
                     << " expected " << (is_legal ? "legal" : "illegal") << " got "
                     << (check_legal ? "legal" : "illegal") << "\n"
                     << flush;

                CHECK(false);
            }

            if (is_legal) {
                Board newboard = board;
                newboard.makeMove(move);
                check(newboard, depth - 1);
            }
        }
    }

    void check_all(const Board& board) {
        check(board, 4);
        CHECK(true);
    }
};

TEST_SUITE("isLegal") {
    TEST_CASE("Standard Chess") {
        const string test_positions[] = {
            "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
            "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ",
            "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - ",
            "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1",
            "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8",
            "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 1",
            "r3k2r/pp1pq1pp/8/4Pp2/b1Q3n1/8/PP1P1PPP/R3K2R w Qkq - 0 1",
            "8/5bk1/8/2Pp4/8/1K6/8/8 w - - 0 1",
            "8/2p3p1/8/q2PKP1P/2N5/8/kp5R/2B5 b - - 0 1",
            "5k2/4N3/8/2Q5/8/q7/8/4K2R w K - 0 1",
            "5rr1/4n2k/4q2P/P1P2n2/3B1p2/4pP2/2N1P3/1RR1K2Q w - - 1 49",
        };

        LegalChecker checker(false);
        checker.init_allmoves();

        Board board;

        for (const auto& fen : test_positions) {
            board.setFen(fen);
            checker.check_all(board);
        }
    }

    TEST_CASE("Chess960") {
        const string dfrc_test_positions[] = {
            "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w AHah - 0 1",
            "1rqbkrbn/1ppppp1p/1n6/p1N3p1/8/2P4P/PP1PPPP1/1RQBKRBN w FBfb - 0 9",
            "rbbqn1kr/pp2p1pp/6n1/2pp1p2/2P4P/P7/BP1PPPP1/R1BQNNKR w HAha - 0 9",
            "rqbbknr1/1ppp2pp/p5n1/4pp2/P7/1PP5/1Q1PPPPP/R1BBKNRN w GAga - 0 9",
            "4rrb1/1kp3b1/1p1p4/pP1Pn2p/5p2/1PR2P2/2P1NB1P/2KR1B2 w D - 0 21",
            "1rkr3b/1ppn3p/3pB1n1/6q1/R2P4/4N1P1/1P5P/2KRQ1B1 b Dbd - 0 14",
            "qbbnrkr1/p1pppppp/1p4n1/8/2P5/6N1/PPNPPPPP/1BRKBRQ1 b FCge - 1 3",
            "rr6/2kpp3/1ppn2p1/p2b1q1p/P4P1P/1PNN2P1/2PP4/1K2R2R b E - 1 20",
            "rr6/2kpp3/1ppn2p1/p2b1q1p/P4P1P/1PNN2P1/2PP4/1K2RR2 w E - 0 20",
            "rr6/2kpp3/1ppnb1p1/p2Q1q1p/P4P1P/1PNN2P1/2PP4/1K2RR2 b E - 2 19",
            "rr6/2kpp3/1ppnb1p1/p4q1p/P4P1P/1PNN2P1/2PP2Q1/1K2RR2 w E - 1 19",
            "rr6/2kpp3/1ppnb1p1/p4q1p/P4P1P/1PNN2P1/2PP2Q1/1K2RR2 w E - 1 19",
            "rr6/2kpp3/1ppnb1p1/p4q1p/P4P1P/1PNN2P1/2PP2Q1/1K2RR2 w E - 1 19",
            "r1kr4/pppppppp/8/8/8/8/PPPPPPPP/5RKR w KQkq - 0 1",
        };

        LegalChecker checker(true);
        checker.init_allmoves();

        Board board;
        board.set960(true);

        for (const auto& fen : dfrc_test_positions) {
            board.setFen(fen);
            checker.check_all(board);
        }
    }
}
