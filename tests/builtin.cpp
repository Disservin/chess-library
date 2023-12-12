#include "../src/include.hpp"
#include "doctest/doctest.hpp"

using namespace chess;

TEST_SUITE("Builtin") {
    TEST_CASE("lsb") {
        Bitboard b = 0x0000000000000001ULL;

        CHECK(b.lsb() == 0);

        b = 0x0000000000000002ULL;
        CHECK(b.lsb() == 1);

        b = 0x0000000000000004ULL;
        CHECK(b.lsb() == 2);
    }

    TEST_CASE("msb") {
        Bitboard b = 0x8000000000000000ULL;

        CHECK(b.msb() == 63);

        b = 0x4000000000000000ULL;
        CHECK(b.msb() == 62);

        b = 0x2000000000000000ULL;
        CHECK(b.msb() == 61);
    }

    TEST_CASE("popcount") {
        Bitboard b = 0x0000000000000001ULL;

        CHECK(b.count() == 1);

        b = 0x0000000000000003ULL;
        CHECK(b.count() == 2);

        b = 0x0000000000000007ULL;
        CHECK(b.count() == 3);
    }
}