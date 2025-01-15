#include "../src/include.hpp"
#include "doctest/doctest.hpp"

using namespace chess;

TEST_SUITE("SAN Parser") {
    TEST_CASE("Test ambiguous pawn capture") {
        auto b = Board{"rnbqkbnr/ppp1p1pp/3p1p2/4N3/8/3P4/PPPKPPPP/R1BQ1BNR b kq - 1 7"};

        Move m = Move::make(Square::underlying::SQ_F6, Square::underlying::SQ_E5);

        CHECK(uci::moveToSan(b, m) == "fxe5");
        CHECK(uci::parseSan(b, "fxe5") == m);
    }

    TEST_CASE("Test ambiguous pawn ep capture") {
        auto b = Board{"rnbqkbnr/pppppp1p/8/5PpP/8/8/PPPPP2P/RNBQKBNR w KQkq g6 0 2"};

        Move m = Move::make<Move::ENPASSANT>(Square::underlying::SQ_F5, Square::underlying::SQ_G6);

        CHECK(uci::moveToSan(b, m) == "fxg6");
        CHECK(uci::parseSan(b, "fxg6") == m);
    }

    TEST_CASE("Test ambiguous knight move") {
        auto b = Board{"rnbqkbnr/ppp3pp/3ppp2/8/8/3P1N1N/PPPKPPPP/R1BQ1B1R w kq - 1 8"};

        Move m = Move::make(Square::underlying::SQ_F3, Square::underlying::SQ_G5);

        CHECK(uci::moveToSan(b, m) == "Nfg5");
        CHECK(uci::parseSan(b, "Nfg5") == m);
    }

    TEST_CASE("Test ambiguous rook move with check") {
        auto b = Board{"4k3/8/8/8/8/8/2R3R1/2K5 w - - 0 1"};

        Move m = Move::make(Square::underlying::SQ_C2, Square::underlying::SQ_E2);

        CHECK(uci::moveToSan(b, m) == "Rce2+");
        CHECK(uci::parseSan(b, "Rce2+") == m);
    }

    TEST_CASE("Test ambiguous rook move with checkmate") {
        auto b = Board{"7k/8/8/8/8/8/2K3R1/3R4 w - - 0 1"};

        Move m = Move::make(Square::underlying::SQ_D1, Square::underlying::SQ_H1);

        CHECK(uci::moveToSan(b, m) == "Rh1#");
        CHECK(uci::parseSan(b, "Rh1#") == m);
    }

    TEST_CASE("Test Knight move") {
        auto b = Board{"rnbqkbnr/ppp1p1pp/3p1p2/8/8/3P1N2/PPPKPPPP/R1BQ1BNR w kq - 0 7"};

        Move m = Move::make(Square::underlying::SQ_F3, Square::underlying::SQ_G5);

        CHECK(uci::moveToSan(b, m) == "Ng5");
        CHECK(uci::parseSan(b, "Ng5") == m);
    }

    TEST_CASE("Test Bishop move") {
        auto b = Board{"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1"};

        Move m = Move::make(Square::underlying::SQ_E1, Square::underlying::SQ_F1);

        CHECK(uci::moveToSan(b, m) == "Kf1");
        CHECK(uci::parseSan(b, "Kf1") == m);
    }

    TEST_CASE("Test Rook move") {
        auto b = Board{"rnbqkbnr/pppppppp/8/8/8/8/PPPPP1PP/R3KR2 w Qkq - 0 1"};

        Move m = Move::make(Square::underlying::SQ_F1, Square::underlying::SQ_F7);

        CHECK(uci::moveToSan(b, m) == "Rxf7");
        CHECK(uci::parseSan(b, "Rxf7") == m);
    }

    TEST_CASE("Test Queen move") {
        auto b = Board{"rnbqkbnr/pppppppp/8/8/8/8/PPPPP1PP/R3KQ1R w KQkq - 0 1"};

        Move m = Move::make(Square::underlying::SQ_F1, Square::underlying::SQ_F7);

        CHECK(uci::moveToSan(b, m) == "Qxf7+");
        CHECK(uci::parseSan(b, "Qxf7+") == m);
    }

    TEST_CASE("Test King move") {
        auto b = Board{"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1"};

        Move m = Move::make(Square::underlying::SQ_E1, Square::underlying::SQ_F1);

        CHECK(uci::moveToSan(b, m) == "Kf1");
        CHECK(uci::parseSan(b, "Kf1") == m);
    }

    TEST_CASE("Test King Castling Short move") {
        auto b = Board{"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQK2R w KQkq - 0 17"};

        Move m = Move::make<Move::CASTLING>(Square::underlying::SQ_E1, Square::underlying::SQ_H1);

        CHECK(uci::moveToSan(b, m) == "O-O");
        CHECK(uci::parseSan(b, "O-O") == m);
    }

    TEST_CASE("Test King Castling Long move") {
        auto b = Board{"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1"};

        Move m = Move::make<Move::CASTLING>(Square::underlying::SQ_E1, Square::underlying::SQ_A1);

        CHECK(uci::moveToSan(b, m) == "O-O-O");
        CHECK(uci::parseSan(b, "O-O-O") == m);
    }

    TEST_CASE("Test King Castling Short move with Zero") {
        auto b = Board{"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQK2R w KQkq - 0 17"};

        Move m = Move::make<Move::CASTLING>(Square::underlying::SQ_E1, Square::underlying::SQ_H1);

        CHECK(uci::parseSan(b, "0-0") == m);
    }

    TEST_CASE("Test King Castling Long move with Zero") {
        auto b = Board{"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1"};

        Move m = Move::make<Move::CASTLING>(Square::underlying::SQ_E1, Square::underlying::SQ_A1);

        CHECK(uci::parseSan(b, "0-0-0") == m);
    }

    TEST_CASE("Test King Castling Short move with Annotation") {
        auto b = Board{"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQK2R w KQkq - 0 17"};

        Move m = Move::make<Move::CASTLING>(Square::underlying::SQ_E1, Square::underlying::SQ_H1);

        CHECK(uci::parseSan(b, "0-0+?!") == m);
    }

    TEST_CASE("Test King Castling Long move with Annotation") {
        auto b = Board{"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1"};

        Move m = Move::make<Move::CASTLING>(Square::underlying::SQ_E1, Square::underlying::SQ_A1);

        CHECK(uci::parseSan(b, "0-0-0+?!") == m);
    }

    TEST_CASE("Test Queen Capture Ambiguity") {
        auto b = Board{"3k4/8/4b3/8/2Q3Q1/8/8/3K4 w - - 0 1"};

        Move m = Move::make(Square::underlying::SQ_C4, Square::underlying::SQ_E6);

        CHECK(uci::parseSan(b, "Qcxe6") == m);
    }

    TEST_CASE("Test Rook Ambiguity") {
        auto b = Board{"3k4/8/8/R7/8/8/8/R3K3 w - - 0 1"};

        Move m = Move::make(Square::underlying::SQ_A1, Square::underlying::SQ_A3);

        CHECK(uci::parseSan(b, "R1a3") == m);
    }

    TEST_CASE("Test Rook Capture Ambiguity") {
        auto b = Board{"2r3k1/4nn2/pq1p1pp1/3Pp3/1pN1P1P1/1P1Q4/P1r1NP2/1K1R3R b - - 2 19"};

        Move m = Move::make(Square::underlying::SQ_C8, Square::underlying::SQ_C4);

        CHECK(uci::parseSan(b, "R8xc4") == m);
    }

    TEST_CASE("Test Knight Capture Ambiguity") {
        auto b = Board{"r5k1/5p2/2n2B1p/4P3/p1n3PN/8/P4PK1/1R6 b - - 2 28"};

        Move m = Move::make(Square::underlying::SQ_C6, Square::underlying::SQ_E5);

        CHECK(uci::parseSan(b, "N6xe5") == m);
    }

    TEST_CASE("Test Pawn Capture Promotion Ambiguity") {
        auto b = Board{"2k2n2/4P1P1/8/8/8/8/8/2K5 w - - 0 1"};

        Move m = Move::make<Move::PROMOTION>(Square::underlying::SQ_E7, Square::underlying::SQ_F8, PieceType::QUEEN);

        CHECK(uci::parseSan(b, "exf8=Q+") == m);
    }

    TEST_CASE("Test Pawn Push") {
        auto b = Board{"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"};

        Move m = Move::make(Square::underlying::SQ_E2, Square::underlying::SQ_E4);

        CHECK(uci::parseSan(b, "e4") == m);
    }

    TEST_CASE("Test Pawn Promotion") {
        auto b = Board{"8/Pk6/8/5p2/8/8/8/2K5 w - - 1 781"};

        Move m = Move::make<Move::PROMOTION>(Square::underlying::SQ_A7, Square::underlying::SQ_A8, PieceType::QUEEN);

        CHECK(uci::parseSan(b, "a8=Q+") == m);
    }

    TEST_CASE("Test Knight Ambiguity") {
        auto b = Board{"8/8/5K2/2N3P1/3N3n/4k3/3N4/7r w - - 59 97"};

        Move m = Move::make(Square::underlying::SQ_D4, Square::underlying::SQ_B3);

        CHECK(uci::parseSan(b, "Nd4b3") == m);
        CHECK(uci::moveToSan(b, m) == "N4b3");
    }

    TEST_CASE("Test Knight Capture Ambiguity") {
        auto b = Board{"8/8/5K2/2N3P1/3N3n/1b2k3/3N4/7r w - - 59 97"};

        Move m = Move::make(Square::underlying::SQ_D4, Square::underlying::SQ_B3);

        CHECK(uci::parseSan(b, "Nd4xb3") == m);
    }

    TEST_CASE("Test Knight Ambiguity 2") {
        auto b = Board{"2N1N3/p7/6k1/1p6/2N1N3/2R5/R3Q1P1/2R3K1 w - - 5 55"};

        {
            Move m = Move::make(Square::underlying::SQ_E4, Square::underlying::SQ_D6);

            CHECK(uci::moveToSan(b, m) == "Ne4d6");
        }

        {
            Move m = Move::make(Square::underlying::SQ_C4, Square::underlying::SQ_D6);

            CHECK(uci::moveToSan(b, m) == "Nc4d6");
        }

        {
            Move m = Move::make(Square::underlying::SQ_C8, Square::underlying::SQ_D6);

            CHECK(uci::moveToSan(b, m) == "Nc8d6");
        }

        {
            Move m = Move::make(Square::underlying::SQ_E8, Square::underlying::SQ_D6);

            CHECK(uci::moveToSan(b, m) == "Ne8d6");
        }
    }

    TEST_CASE("Parse No Move") {
        Board b = Board();

        CHECK(uci::parseSan(b, "") == Move::NO_MOVE);
    }
}