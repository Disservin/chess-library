#include "../include/chess.hpp"
#include "doctest/doctest.hpp"

using namespace chess;

TEST_SUITE("SAN Parser") {
    TEST_CASE("Test ambiguous pawn capture") {
        Board b;
        b.setFen("rnbqkbnr/ppp1p1pp/3p1p2/4N3/8/3P4/PPPKPPPP/R1BQ1BNR b kq - 1 7");

        Move m = Move::make(SQ_F6, SQ_E5);

        CHECK(uci::moveToSan(b, m) == "fxe5");
        CHECK(uci::parseSan(b, "fxe5") == m);
    }

    TEST_CASE("Test ambiguous pawn ep capture") {
        Board b;
        b.setFen("rnbqkbnr/pppppp1p/8/5PpP/8/8/PPPPP2P/RNBQKBNR w KQkq g6 0 2");

        Move m = Move::make<Move::ENPASSANT>(SQ_F5, SQ_G6);

        CHECK(uci::moveToSan(b, m) == "fxg6");
        CHECK(uci::parseSan(b, "fxg6") == m);
    }

    TEST_CASE("Test ambiguous knight move") {
        Board b;
        b.setFen("rnbqkbnr/ppp3pp/3ppp2/8/8/3P1N1N/PPPKPPPP/R1BQ1B1R w kq - 1 8");

        Move m = Move::make(SQ_F3, SQ_G5);

        CHECK(uci::moveToSan(b, m) == "Nfg5");
        CHECK(uci::parseSan(b, "Nfg5") == m);
    }

    TEST_CASE("Test ambiguous rook move with check") {
        Board b;
        b.setFen("4k3/8/8/8/8/8/2R3R1/2K5 w - - 0 1");

        Move m = Move::make(SQ_C2, SQ_E2);

        CHECK(uci::moveToSan(b, m) == "Rce2+");
        CHECK(uci::parseSan(b, "Rce2+") == m);
    }

    TEST_CASE("Test ambiguous rook move with checkmate") {
        Board b;
        b.setFen("7k/8/8/8/8/8/2K3R1/3R4 w - - 0 1");

        Move m = Move::make(SQ_D1, SQ_H1);

        CHECK(uci::moveToSan(b, m) == "Rh1#");
        CHECK(uci::parseSan(b, "Rh1#") == m);
    }

    TEST_CASE("Test Knight move") {
        Board b;
        b.setFen("rnbqkbnr/ppp1p1pp/3p1p2/8/8/3P1N2/PPPKPPPP/R1BQ1BNR w kq - 0 7");

        Move m = Move::make(SQ_F3, SQ_G5);

        CHECK(uci::moveToSan(b, m) == "Ng5");
        CHECK(uci::parseSan(b, "Ng5") == m);
    }

    TEST_CASE("Test Bishop move") {
        Board b;
        b.setFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1");

        Move m = Move::make(SQ_E1, SQ_F1);

        CHECK(uci::moveToSan(b, m) == "Kf1");
        CHECK(uci::parseSan(b, "Kf1") == m);
    }

    TEST_CASE("Test Rook move") {
        Board b;
        b.setFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPP1PP/R3KR2 w Qkq - 0 1");

        Move m = Move::make(SQ_F1, SQ_F7);

        CHECK(uci::moveToSan(b, m) == "Rxf7");
        CHECK(uci::parseSan(b, "Rxf7") == m);
    }

    TEST_CASE("Test Queen move") {
        Board b;
        b.setFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPP1PP/R3KQ1R w KQkq - 0 1");

        Move m = Move::make(SQ_F1, SQ_F7);

        CHECK(uci::moveToSan(b, m) == "Qxf7+");
        CHECK(uci::parseSan(b, "Qxf7+") == m);
    }

    TEST_CASE("Test King move") {
        Board b;
        b.setFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1");

        Move m = Move::make(SQ_E1, SQ_F1);

        CHECK(uci::moveToSan(b, m) == "Kf1");
        CHECK(uci::parseSan(b, "Kf1") == m);
    }

    TEST_CASE("Test King Castling Short move") {
        Board b;
        b.setFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQK2R w KQkq - 0 17");

        Move m = Move::make<Move::CASTLING>(SQ_E1, SQ_H1);

        CHECK(uci::moveToSan(b, m) == "O-O");
        CHECK(uci::parseSan(b, "O-O") == m);
    }

    TEST_CASE("Test King Castling Long move") {
        Board b;
        b.setFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1");

        Move m = Move::make<Move::CASTLING>(SQ_E1, SQ_A1);

        CHECK(uci::moveToSan(b, m) == "O-O-O");
        CHECK(uci::parseSan(b, "O-O-O") == m);
    }

    TEST_CASE("Test King Castling Short move with Zero") {
        Board b;
        b.setFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQK2R w KQkq - 0 17");

        Move m = Move::make<Move::CASTLING>(SQ_E1, SQ_H1);

        CHECK(uci::parseSan(b, "0-0") == m);
    }

    TEST_CASE("Test King Castling Long move with Zero") {
        Board b;
        b.setFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1");

        Move m = Move::make<Move::CASTLING>(SQ_E1, SQ_A1);

        CHECK(uci::parseSan(b, "0-0-0") == m);
    }

    TEST_CASE("Test King Castling Short move with Annotation") {
        Board b;
        b.setFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQK2R w KQkq - 0 17");

        Move m = Move::make<Move::CASTLING>(SQ_E1, SQ_H1);

        CHECK(uci::parseSan(b, "0-0+?!") == m);
    }

    TEST_CASE("Test King Castling Long move with Annotation") {
        Board b;
        b.setFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1");

        Move m = Move::make<Move::CASTLING>(SQ_E1, SQ_A1);

        CHECK(uci::parseSan(b, "0-0-0+?!") == m);
    }

    TEST_CASE("Test Queen Capture Ambiguity") {
        Board b;
        b.setFen("3k4/8/4b3/8/2Q3Q1/8/8/3K4 w - - 0 1");

        Move m = Move::make(SQ_C4, SQ_E6);

        CHECK(uci::parseSan(b, "Qcxe6") == m);
    }

    TEST_CASE("Test Rook Ambiguity") {
        Board b;
        b.setFen("3k4/8/8/R7/8/8/8/R3K3 w - - 0 1");

        Move m = Move::make(SQ_A1, SQ_A3);

        CHECK(uci::parseSan(b, "R1a3") == m);
    }

    TEST_CASE("Test Rook Capture Ambiguity") {
        Board b;
        b.setFen("2r3k1/4nn2/pq1p1pp1/3Pp3/1pN1P1P1/1P1Q4/P1r1NP2/1K1R3R b - - 2 19");

        Move m = Move::make(SQ_C8, SQ_C4);

        CHECK(uci::parseSan(b, "R8xc4") == m);
    }

    TEST_CASE("Test Pawn Capture Promotion Ambiguity") {
        Board b;
        b.setFen("2k2n2/4P1P1/8/8/8/8/8/2K5 w - - 0 1");

        Move m = Move::make<Move::PROMOTION>(SQ_E7, SQ_F8, PieceType::QUEEN);

        CHECK(uci::parseSan(b, "exf8=Q+") == m);
    }

    TEST_CASE("Test Pawn Push") {
        Board b;
        b.setFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

        Move m = Move::make(SQ_E2, SQ_E4);

        CHECK(uci::parseSan(b, "e4") == m);
    }
}