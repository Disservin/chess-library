#include "../src/include.hpp"
#include "doctest/doctest.hpp"

using namespace chess;

TEST_SUITE("Zobrist Hash") {
    TEST_CASE("Test Zobrist Hash Startpos") {
        Board b;

        b.setFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
        CHECK(b.hash() == 0x463b96181691fc9c);
        CHECK(b.hash() == 0x463b96181691fc9c);

        auto mv = uci::uciToMove(b, "e2e4");
        CHECK(b.zobristAfter<true>(mv) == 0x823c9b50fd114196);
        CHECK(b.zobristAfter<false>(mv) == 0x823c9b50fd114196);
        b.makeMove(mv);
        CHECK(b.hash() == 0x823c9b50fd114196);
        CHECK(b.hash() == 0x823c9b50fd114196);

        mv = uci::uciToMove(b, "d7d5");
        CHECK(b.zobristAfter<true>(mv) == 0x0756b94461c50fb0);
        CHECK(b.zobristAfter<false>(mv) == 0x0756b94461c50fb0);
        b.makeMove(mv);
        CHECK(b.hash() == 0x0756b94461c50fb0);
        CHECK(b.hash() == 0x0756b94461c50fb0);

        mv = uci::uciToMove(b, "e4e5");
        CHECK(b.zobristAfter<true>(mv) == 0x662fafb965db29d4);
        CHECK(b.zobristAfter<false>(mv) == 0x662fafb965db29d4);
        b.makeMove(mv);
        CHECK(b.hash() == 0x662fafb965db29d4);
        CHECK(b.hash() == 0x662fafb965db29d4);

        mv = uci::uciToMove(b, "f7f5");
        CHECK(b.zobristAfter<true>(mv) == 0x22a48b5a8e47ff78);
        b.makeMove(mv);
        CHECK(b.hash() == 0x22a48b5a8e47ff78);
        CHECK(b.hash() == 0x22a48b5a8e47ff78);

        mv = uci::uciToMove(b, "e1e2");
        CHECK(b.zobristAfter<true>(mv) == 0x652a607ca3f242c1);
        b.makeMove(mv);
        CHECK(b.hash() == 0x652a607ca3f242c1);
        CHECK(b.hash() == 0x652a607ca3f242c1);

        mv = uci::uciToMove(b, "e8f7");
        CHECK(b.zobristAfter<true>(mv) == 0x00fdd303c946bdd9);
        b.makeMove(mv);
        CHECK(b.hash() == 0x00fdd303c946bdd9);
        CHECK(b.hash() == 0x00fdd303c946bdd9);
    }

    TEST_CASE("Test Zobrist Hash Second Position") {
        Board b;

        b.setFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

        {
            auto mv = uci::uciToMove(b, "a2a4");
            CHECK(b.zobristAfter<true>(mv) == 0x2DF2E8F47B022952);
            CHECK(b.zobristAfter<false>(mv) == 0x2DF2E8F47B022952);
            b.makeMove(mv);

            mv = uci::uciToMove(b, "b7b5");
            CHECK(b.zobristAfter<true>(mv) == 0x4DF682E1E0AF946F);
            CHECK(b.zobristAfter<false>(mv) == 0x4DF682E1E0AF946F);
            b.makeMove(mv);

            mv = uci::uciToMove(b, "h2h4");
            CHECK(b.zobristAfter<true>(mv) == 0xD1551EC84B90ED11);
            CHECK(b.zobristAfter<false>(mv) == 0xD1551EC84B90ED11);
            b.makeMove(mv);

            mv = uci::uciToMove(b, "b5b4");
            CHECK(b.zobristAfter<true>(mv) == 0xB0982F168A89B452);
            CHECK(b.zobristAfter<false>(mv) == 0xB0982F168A89B452);
            b.makeMove(mv);

            mv = uci::uciToMove(b, "c2c4");
            CHECK(b.zobristAfter<true>(mv) == 0x3C8123EA7B067637);
            b.makeMove(mv);

            CHECK(b.hash() == 0x3c8123ea7b067637);
        }

        {
            auto mv = uci::uciToMove(b, "b4c3");
            CHECK(b.zobristAfter<true>(mv) == 0x93D32682782EDFAE);
            CHECK(b.zobristAfter<false>(mv) == 0x93D32682782EDFAE);
            b.makeMove(mv);

            mv = uci::uciToMove(b, "a1a3");
            CHECK(b.zobristAfter<true>(mv) == 0x5c3f9b829b279560);
            b.makeMove(mv);

            CHECK(b.hash() == 0x5c3f9b829b279560);
        }
    }

    TEST_CASE("Test Zobrist Hash White Castling") {
        Board b;

        {
            b.setFen("r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1");
            auto mv = uci::uciToMove(b, "e1g1");
            CHECK(b.zobristAfter<true>(mv) == 9500135572247264067ull);
            b.makeMove(mv);
            CHECK(b.hash() == 9500135572247264067ull);

            b.setFen("r3k2r/8/8/8/8/8/8/R4RK1 b kq - 1 1");
            CHECK(b.hash() == 9500135572247264067ull);
        }

        {
            b.setFen("r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1");
            auto mv = uci::uciToMove(b, "e1c1");
            CHECK(b.zobristAfter<true>(mv) == 14235734314054086603ull);
            b.makeMove(mv);
            CHECK(b.hash() == 14235734314054086603ull);

            b.setFen("r3k2r/8/8/8/8/8/8/2KR3R b kq - 1 1");
            CHECK(b.hash() == 14235734314054086603ull);
        }

        {
            b.setFen("r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1");
            auto mv = uci::uciToMove(b, "e1f2");
            CHECK(b.zobristAfter<true>(mv) == 9624187742021389814ull);
            b.makeMove(mv);
            CHECK(b.hash() == 9624187742021389814ull);

            b.setFen("r3k2r/8/8/8/8/8/5K2/R6R b kq - 1 1");
            CHECK(b.hash() == 9624187742021389814ull);
        }

        {
            b.setFen("r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1");
            auto mv = uci::uciToMove(b, "e1d2");
            CHECK(b.zobristAfter<true>(mv) == 9930369406275139450ull);
            b.makeMove(mv);
            CHECK(b.hash() == 9930369406275139450ull);

            b.setFen("r3k2r/8/8/8/8/8/3K4/R6R b kq - 1 1");
            CHECK(b.hash() == 9930369406275139450ull);
        }

        {
            b.setFen("r3k3/8/8/8/8/8/8/4K2R w Kq - 0 1");
            auto mv = uci::uciToMove(b, "h1h2");
            CHECK(b.zobristAfter<true>(mv) == 16699550102102435117ull);
            b.makeMove(mv);
            CHECK(b.hash() == 16699550102102435117ull);

            b.setFen("r3k3/8/8/8/8/8/7R/4K3 b q - 1 1");
            CHECK(b.hash() == 16699550102102435117ull);
        }
    }

    TEST_CASE("Test Zobrist Hash Black Castling") {
        Board b;

        {
            b.setFen("r3k2r/8/8/8/8/8/8/R3K2R b KQkq - 0 1");
            auto mv = uci::uciToMove(b, "e8g8");
            CHECK(b.zobristAfter<true>(mv) == 1143055385231625393ull);
            b.makeMove(mv);
            CHECK(b.hash() == 1143055385231625393ull);

            b.setFen("r4rk1/8/8/8/8/8/8/R3K2R w KQ - 1 2");
            CHECK(b.hash() == 1143055385231625393ull);
        }

        {
            b.setFen("r3k2r/8/8/8/8/8/8/R3K2R b KQkq - 0 1");
            auto mv = uci::uciToMove(b, "e8c8");
            CHECK(b.zobristAfter<true>(mv) == 3909839401934106976ull);
            b.makeMove(mv);
            CHECK(b.hash() == 3909839401934106976ull);

            b.setFen("2kr3r/8/8/8/8/8/8/R3K2R w KQ - 1 2");
            CHECK(b.hash() == 3909839401934106976ull);
        }

        {
            b.setFen("r3k2r/8/8/8/8/8/8/R3K2R b KQkq - 0 1");
            auto mv = uci::uciToMove(b, "e8f7");
            CHECK(b.zobristAfter<true>(mv) == 6963598675116890690ull);
            b.makeMove(mv);
            CHECK(b.hash() == 6963598675116890690ull);

            b.setFen("r6r/5k2/8/8/8/8/8/R3K2R w KQ - 1 2");
            CHECK(b.hash() == 6963598675116890690ull);
        }

        {
            b.setFen("r3k2r/8/8/8/8/8/8/R3K2R b KQkq - 0 1");
            auto mv = uci::uciToMove(b, "e8d7");
            CHECK(b.zobristAfter<true>(mv) == 6266296883909487836ull);
            b.makeMove(mv);
            CHECK(b.hash() == 6266296883909487836ull);

            b.setFen("r6r/3k4/8/8/8/8/8/R3K2R w KQ - 1 2");
            CHECK(b.hash() == 6266296883909487836ull);
        }

        {
            b.setFen("r3k3/8/8/3B4/8/8/8/4K3 w q - 0 1");
            auto mv = uci::uciToMove(b, "d5a8");
            CHECK(b.zobristAfter<true>(mv) == 11170087546614790902ull);
            b.makeMove(mv);
            CHECK(b.hash() == 11170087546614790902ull);

            b.setFen("B3k3/8/8/8/8/8/8/4K3 b - - 0 1");
            CHECK(b.hash() == 11170087546614790902ull);
        }

        {
            b.setFen("r3k3/8/8/8/8/8/8/4K2R b Kq - 0 1");
            auto mv = uci::uciToMove(b, "a8a7");
            CHECK(b.zobristAfter<true>(mv) == 16038026699965099486ull);
            b.makeMove(mv);
            CHECK(b.hash() == 16038026699965099486ull);

            b.setFen("4k3/r7/8/8/8/8/8/4K2R w K - 1 2");
            CHECK(b.hash() == 16038026699965099486ull);
        }
    }

    TEST_CASE("Test Zobrist Hash Enpassant") {
        Board b;

        {
            b.setFen("rnbqkbnr/pppppppp/8/5P2/8/8/PPPPP1PP/RNBQKBNR b KQkq - 0 1");
            auto mv = uci::uciToMove(b, "e7e5");
            CHECK(b.zobristAfter<true>(mv) == 7868419208351108928ull);
            b.makeMove(mv);
            CHECK(b.hash() == 7868419208351108928ull);

            b.setFen("rnbqkbnr/pppp1ppp/8/4pP2/8/8/PPPPP1PP/RNBQKBNR w KQkq e6 0 2");
            CHECK(b.hash() == 7868419208351108928ull);
        }

        {
            b.setFen("rnbqkbnr/pppp1ppp/8/4pP2/8/8/PPPPP1PP/RNBQKBNR w KQkq e6 0 2");
            auto mv = uci::uciToMove(b, "f5e6");
            CHECK(b.zobristAfter<true>(mv) == 14328713941299708535ull);
            b.makeMove(mv);
            CHECK(b.hash() == 14328713941299708535ull);

            b.setFen("rnbqkbnr/pppp1ppp/4P3/8/8/8/PPPPP1PP/RNBQKBNR b KQkq - 0 2");
            CHECK(b.hash() == 14328713941299708535ull);
        }

        {
            b.setFen("rnbqkbnr/pppp1ppp/8/4pP2/8/8/PPPPP1PP/RNBQKBNR w KQkq e6 0 2");
            auto mv = uci::uciToMove(b, "e2e3");
            CHECK(b.zobristAfter<true>(mv) == 1093876600604329325ull);
            b.makeMove(mv);
            CHECK(b.hash() == 1093876600604329325ull);

            b.setFen("rnbqkbnr/pppp1ppp/8/4pP2/8/4P3/PPPP2PP/RNBQKBNR b KQkq - 0 2");
            CHECK(b.hash() == 1093876600604329325ull);
        }

        {
            b.setFen("rnbqkbnr/pppp1ppp/8/4pP2/8/8/PPPPP1PP/RNBQKBNR w KQkq e6 0 2");
            auto mv = uci::uciToMove(b, "f5f6");
            CHECK(b.zobristAfter<true>(mv) == 2265987269840498900ull);
            b.makeMove(mv);
            CHECK(b.hash() == 2265987269840498900ull);

            b.setFen("rnbqkbnr/pppp1ppp/5P2/4p3/8/8/PPPPP1PP/RNBQKBNR b KQkq - 0 2");
            CHECK(b.hash() == 2265987269840498900ull);
        }
    }
}
