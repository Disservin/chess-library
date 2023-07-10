#include "../src/chess.hpp"
#include "doctest/doctest.hpp"

using namespace chess;

TEST_CASE("Test ambiguous pawn capture") {
    Board b;
    b.setFen("rnbqkbnr/ppp1p1pp/3p1p2/4N3/8/3P4/PPPKPPPP/R1BQ1BNR b kq - 1 7");

    Move m = Move::make(SQ_F6, SQ_E5);

    CHECK(uci::moveToSan(b, m) == "fxe5");
}

TEST_CASE("Test ambiguous pawn ep capture") {
    Board b;
    b.setFen("rnbqkbnr/pppppp1p/8/5PpP/8/8/PPPPP2P/RNBQKBNR w KQkq g6 0 2");

    Move m = Move::make<Move::ENPASSANT>(SQ_F5, SQ_G6);

    CHECK(uci::moveToSan(b, m) == "fxg6");
}

TEST_CASE("Test ambiguous knight move") {
    Board b;
    b.setFen("rnbqkbnr/ppp3pp/3ppp2/8/8/3P1N1N/PPPKPPPP/R1BQ1B1R w kq - 1 8");

    Move m = Move::make(SQ_F3, SQ_G5);

    CHECK(uci::moveToSan(b, m) == "Nfg5");
}

TEST_CASE("Test ambiguous rook move with check") {
    Board b;
    b.setFen("4k3/8/8/8/8/8/2R3R1/2K5 w - - 0 1");

    Move m = Move::make(SQ_C2, SQ_E2);

    CHECK(uci::moveToSan(b, m) == "Rce2+");
}

TEST_CASE("Test ambiguous rook move with checkmate") {
    Board b;
    b.setFen("7k/8/8/8/8/8/2K3R1/3R4 w - - 0 1");

    Move m = Move::make(SQ_D1, SQ_H1);

    CHECK(uci::moveToSan(b, m) == "Rh1#");
}

TEST_CASE("Test Knight move") {
    Board b;
    b.setFen("rnbqkbnr/ppp1p1pp/3p1p2/8/8/3P1N2/PPPKPPPP/R1BQ1BNR w kq - 0 7");

    Move m = Move::make(SQ_F3, SQ_G5);

    CHECK(uci::moveToSan(b, m) == "Ng5");
}

TEST_CASE("Test Bishop move") {
    Board b;
    b.setFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1");

    Move m = Move::make(SQ_E1, SQ_F1);

    CHECK(uci::moveToSan(b, m) == "Kf1");
}

TEST_CASE("Test Rook move") {
    Board b;
    b.setFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPP1PP/R3KR2 w Qkq - 0 1");

    Move m = Move::make(SQ_F1, SQ_F7);

    CHECK(uci::moveToSan(b, m) == "Rxf7");
}

TEST_CASE("Test Queen move") {
    Board b;
    b.setFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPP1PP/R3KQ1R w KQkq - 0 1");

    Move m = Move::make(SQ_F1, SQ_F7);

    CHECK(uci::moveToSan(b, m) == "Qxf7+");
}

TEST_CASE("Test King move") {
    Board b;
    b.setFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1");

    Move m = Move::make(SQ_E1, SQ_F1);

    CHECK(uci::moveToSan(b, m) == "Kf1");
}

TEST_CASE("Test King Castling Short move") {
    Board b;
    b.setFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQK2R w KQkq - 0 17");

    Move m = Move::make<Move::CASTLING>(SQ_E1, SQ_G1);

    CHECK(uci::moveToSan(b, m) == "O-O");
}

TEST_CASE("Test King Castling Short move") {
    Board b;
    b.setFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1");

    Move m = Move::make<Move::CASTLING>(SQ_E1, SQ_C1);

    CHECK(uci::moveToSan(b, m) == "O-O-O");
}