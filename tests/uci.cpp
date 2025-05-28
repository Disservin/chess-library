#include "../src/include.hpp"
#include "doctest/doctest.hpp"

using namespace chess;

TEST_SUITE("UCI Move Conversion") {
    TEST_CASE("Test ambiguous pawn capture") {
        Board b;
        b.setFen("rnbqkbnr/ppp1p1pp/3p1p2/4N3/8/3P4/PPPKPPPP/R1BQ1BNR b kq - 1 7");

        std::string uci = "f6e5";

        CHECK(uci::uciToMove(b, uci) == Move::make(Square::SQ_F6, Square::SQ_E5));
    }

    TEST_CASE("Test ambiguous pawn ep capture") {
        Board b;
        b.setFen("rnbqkbnr/pppppp1p/8/5PpP/8/8/PPPPP2P/RNBQKBNR w KQkq g6 0 2");

        std::string uci = "f5g6";

        CHECK(uci::uciToMove(b, uci) == Move::make<Move::ENPASSANT>(Square::SQ_F5, Square::SQ_G6));
    }

    TEST_CASE("Test ambiguous knight move") {
        Board b;
        b.setFen("rnbqkbnr/ppp3pp/3ppp2/8/8/3P1N1N/PPPKPPPP/R1BQ1B1R w kq - 1 8");

        std::string uci = "f3g5";

        CHECK(uci::uciToMove(b, uci) == Move::make(Square::SQ_F3, Square::SQ_G5));
    }

    TEST_CASE("Test ambiguous rook move with check") {
        Board b;
        b.setFen("4k3/8/8/8/8/8/2R3R1/2K5 w - - 0 1");

        std::string uci = "c2e2+";

        CHECK(uci::uciToMove(b, uci) == Move::NO_MOVE);
    }

    TEST_CASE("Test ambiguous rook move with checkmate") {
        Board b;
        b.setFen("7k/8/8/8/8/8/2K3R1/3R4 w - - 0 1");

        std::string uci = "d1h1";

        CHECK(uci::uciToMove(b, uci) == Move::make(Square::SQ_D1, Square::SQ_H1));
    }

    TEST_CASE("Test Knight move") {
        Board b;
        b.setFen("rnbqkbnr/ppp1p1pp/3p1p2/8/8/3P1N2/PPPKPPPP/R1BQ1BNR w kq - 0 7");

        std::string uci = "f3g5";

        CHECK(uci::uciToMove(b, uci) == Move::make(Square::SQ_F3, Square::SQ_G5));
    }

    TEST_CASE("Test illegal pawn promotion") {
        Board b;
        b.setFen("rnbqkbnr/ppp1p1pp/3p1p2/8/8/3P4/PPPKPPPP/R1BQ1BNR w kq - 0 7");

        std::string uci = "f7f8q";

        CHECK(uci::uciToMove(b, uci) == Move::NO_MOVE);
    }

    TEST_CASE("Throws on invalid move") {
        Board b;
        b.setFen("rnbqkbnr/ppp1p1pp/3p1p2/8/8/3P4/PPPKPPPP/R1BQ1BNR w kq - 0 7");

        std::string uci = "ff8";

        CHECK(uci::uciToMove(b, uci) == Move::NO_MOVE);
    }

    TEST_CASE("Throws on invalid promotion") {
        Board b;
        b.setFen("rnbqkbnr/ppp1p1pp/3p1p2/8/8/3P4/PPPKPPPP/R1BQ1BNR w kq - 0 7");

        std::string uci = "f7f82";

        CHECK(uci::uciToMove(b, uci) == Move::NO_MOVE);
    }

    TEST_CASE("Black castling") {
        Board b;
        b.setFen("r1b1k2r/pppp1ppp/2nbp3/5n2/6Q1/2N1PN2/PPPP1PPP/R1B1K2R b KQkq - 3 8");
        std::string uci = "e8g8";

        b.makeMove(uci::uciToMove(b, uci));
        CHECK(b.getFen()=="r1b2rk1/pppp1ppp/2nbp3/5n2/6Q1/2N1PN2/PPPP1PPP/R1B1K2R w KQ - 4 9");
    }
    TEST_CASE("White castling") {
        Board b;
        b.setFen("r1b2rk1/pppp1ppp/2nbp3/5n2/6Q1/2N1PN2/PPPP1PPP/R1B1K2R w KQ - 4 9");
        std::string uci = "e1g1";

        b.makeMove(uci::uciToMove(b, uci));
        CHECK(b.getFen()=="r1b2rk1/pppp1ppp/2nbp3/5n2/6Q1/2N1PN2/PPPP1PPP/R1B2RK1 b - - 5 9");
    }
    TEST_CASE("Movelist white castling") {
        chess::Board board;
        std::istringstream iss("e2e3 e7e6 d1g4 d8f6 f1d3 b8c6 b1c3 g8h6 g4h5 f6f5 "
                                "d3f5 h6f5 g1f3 f8d6 h5g4 e8g8 e1h1");
        std::string token;
        while (iss >> token) {
            chess::Move mv = chess::uci::uciToMove(board, token);
            board.makeMove(mv);
        }
        CHECK(board.getFen()=="r1b2rk1/pppp1ppp/2nbp3/5n2/6Q1/2N1PN2/PPPP1PPP/R1B2RK1 b - - 5 9");
    }
}

TEST_SUITE("UCI isUciMove Check") {
    TEST_CASE("Test valid standard moves") {
        CHECK(uci::isUciMove("e2e4"));  // Pawn push
        CHECK(uci::isUciMove("g1f3"));  // Knight move
        CHECK(uci::isUciMove("f1c4"));  // Bishop move
        CHECK(uci::isUciMove("h1g1"));  // Rook move
        CHECK(uci::isUciMove("d1d7"));  // Queen move
        CHECK(uci::isUciMove("e1e2"));  // King move
    }

    TEST_CASE("Test valid pawn promotion moves") {
        CHECK(uci::isUciMove("e7e8q"));  // Promote to queen
        CHECK(uci::isUciMove("a7a8r"));  // Promote to rook
        CHECK(uci::isUciMove("h2h1n"));  // Promote to knight
        CHECK(uci::isUciMove("c7c8b"));  // Promote to bishop
    }

    TEST_CASE("Test invalid formats") {
        CHECK_FALSE(uci::isUciMove(""));        // Empty string
        CHECK_FALSE(uci::isUciMove("e2e"));     // Too short
        CHECK_FALSE(uci::isUciMove("e2e4 "));   // Additional space
        CHECK_FALSE(uci::isUciMove("e2e4e5"));  // Too long
        CHECK_FALSE(uci::isUciMove("e2-e4"));   // Invalid format with hyphen
        CHECK_FALSE(uci::isUciMove("P@e4"));    // Not UCI format
        CHECK_FALSE(uci::isUciMove("O-O"));     // Castling in algebraic, not UCI
    }

    TEST_CASE("Test invalid coordinates") {
        CHECK_FALSE(uci::isUciMove("i2e4"));  // Invalid file 'i'
        CHECK_FALSE(uci::isUciMove("e9e4"));  // Invalid rank '9'
        CHECK_FALSE(uci::isUciMove("e0e4"));  // Invalid rank '0'
        CHECK_FALSE(uci::isUciMove("e2j4"));  // Invalid file 'j'
        CHECK_FALSE(uci::isUciMove("22e4"));  // Not a file
        CHECK_FALSE(uci::isUciMove("e2e/"));  // Not a rank
    }

    TEST_CASE("Test case sensitivity") {
        CHECK_FALSE(uci::isUciMove("E2E4"));   // Uppercase coordinates
        CHECK_FALSE(uci::isUciMove("e7e8Q"));  // Uppercase promotion
        CHECK_FALSE(uci::isUciMove("e7e8X"));  // Invalid promotion piece
    }

    TEST_CASE("Test special UCI moves") {
        // Some engines use special notations for castling
        CHECK(uci::isUciMove("e1g1"));  // King-side castling for white
        CHECK(uci::isUciMove("e8g8"));  // King-side castling for black
        CHECK(uci::isUciMove("e1c1"));  // Queen-side castling for white
        CHECK(uci::isUciMove("e8c8"));  // Queen-side castling for black
    }

    TEST_CASE("Test edge cases") {
        CHECK_FALSE(uci::isUciMove("e7e9q"));  // Invalid rank promotion
        CHECK_FALSE(uci::isUciMove("e7k8q"));  // Invalid file promotion
    }
}