#include "../src/include.hpp"
#include "doctest/doctest.hpp"

using namespace chess;

TEST_SUITE("UCI Move Conversion") {
    TEST_CASE("Test ambiguous pawn capture") {
        Board b;
        b.setFen("rnbqkbnr/ppp1p1pp/3p1p2/4N3/8/3P4/PPPKPPPP/R1BQ1BNR b kq - 1 7");

        std::string uci = "f6e5";

        CHECK(uci::uciToMove(b, uci) == Move::make(Square::underlying::SQ_F6, Square::underlying::SQ_E5));
    }

    TEST_CASE("Test ambiguous pawn ep capture") {
        Board b;
        b.setFen("rnbqkbnr/pppppp1p/8/5PpP/8/8/PPPPP2P/RNBQKBNR w KQkq g6 0 2");

        std::string uci = "f5g6";

        CHECK(uci::uciToMove(b, uci) ==
              Move::make<Move::ENPASSANT>(Square::underlying::SQ_F5, Square::underlying::SQ_G6));
    }

    TEST_CASE("Test ambiguous knight move") {
        Board b;
        b.setFen("rnbqkbnr/ppp3pp/3ppp2/8/8/3P1N1N/PPPKPPPP/R1BQ1B1R w kq - 1 8");

        std::string uci = "f3g5";

        CHECK(uci::uciToMove(b, uci) == Move::make(Square::underlying::SQ_F3, Square::underlying::SQ_G5));
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

        CHECK(uci::uciToMove(b, uci) == Move::make(Square::underlying::SQ_D1, Square::underlying::SQ_H1));
    }

    TEST_CASE("Test Knight move") {
        Board b;
        b.setFen("rnbqkbnr/ppp1p1pp/3p1p2/8/8/3P1N2/PPPKPPPP/R1BQ1BNR w kq - 0 7");

        std::string uci = "f3g5";

        CHECK(uci::uciToMove(b, uci) == Move::make(Square::underlying::SQ_F3, Square::underlying::SQ_G5));
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
}