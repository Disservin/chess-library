#include "../include/chess.hpp"
#include "doctest/doctest.hpp"

using namespace chess;

TEST_SUITE("Builtin") {
    TEST_CASE("lsb") {
        U64 b = 0x0000000000000001ULL;

        CHECK(builtin::lsb(b) == 0);

        b = 0x0000000000000002ULL;
        CHECK(builtin::lsb(b) == 1);

        b = 0x0000000000000004ULL;
        CHECK(builtin::lsb(b) == 2);
    }

    TEST_CASE("msb") {
        U64 b = 0x8000000000000000ULL;

        CHECK(builtin::msb(b) == 63);

        b = 0x4000000000000000ULL;
        CHECK(builtin::msb(b) == 62);

        b = 0x2000000000000000ULL;
        CHECK(builtin::msb(b) == 61);
    }

    TEST_CASE("popcount") {
        U64 b = 0x0000000000000001ULL;

        CHECK(builtin::popcount(b) == 1);

        b = 0x0000000000000003ULL;
        CHECK(builtin::popcount(b) == 2);

        b = 0x0000000000000007ULL;
        CHECK(builtin::popcount(b) == 3);
    }
}