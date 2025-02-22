#include "../src/include.hpp"
#include "doctest/doctest.hpp"

using namespace chess;

TEST_SUITE("Bitboard") {
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

    TEST_CASE("pop") {
        Bitboard b = 0x0000000000000001ULL;

        CHECK(b.pop() == 0);
        CHECK(b == 0);

        b = 0x0000000000000003ULL;
        CHECK(b.pop() == 0);
        CHECK(b == 0x0000000000000002ULL);

        b = 0x0000000000000007ULL;
        CHECK(b.pop() == 0);
        CHECK(b == 0x0000000000000006ULL);
    }

    TEST_CASE("getBits") {
        Bitboard b = 0x0000000000000001ULL;

        CHECK(b.getBits() == 0x0000000000000001ULL);

        b = 0x0000000000000003ULL;
        CHECK(b.getBits() == 0x0000000000000003ULL);

        b = 0x0000000000000007ULL;
        CHECK(b.getBits() == 0x0000000000000007ULL);
    }

    TEST_CASE("empty") {
        Bitboard b = 0x0000000000000000ULL;

        CHECK(b.empty());

        b = 0x0000000000000001ULL;
        CHECK(!b.empty());

        b = 0x0000000000000003ULL;
        CHECK(!b.empty());
    }

    TEST_CASE("operator==") {
        Bitboard b = 0x0000000000000000ULL;

        CHECK(b == 0x0000000000000000ULL);

        b = 0x0000000000000001ULL;
        CHECK(b == 0x0000000000000001ULL);

        b = 0x0000000000000003ULL;
        CHECK(b == 0x0000000000000003ULL);
    }

    TEST_CASE("operator!=") {
        Bitboard b = 0x0000000000000000ULL;

        CHECK(b != 0x0000000000000001ULL);

        b = 0x0000000000000001ULL;
        CHECK(b != 0x0000000000000002ULL);

        b = 0x0000000000000003ULL;
        CHECK(b != 0x0000000000000004ULL);
    }

    TEST_CASE("operator&") {
        Bitboard b = 0x0000000000000000ULL;

        CHECK((b & 0x0000000000000000ULL) == 0x0000000000000000ULL);

        b = 0x0000000000000001ULL;
        CHECK((b & 0x0000000000000001ULL) == 0x0000000000000001ULL);

        b = 0x0000000000000003ULL;
        CHECK((b & 0x0000000000000003ULL) == 0x0000000000000003ULL);
    }

    TEST_CASE("operator|") {
        Bitboard b = 0x0000000000000000ULL;

        CHECK((b | 0x0000000000000000ULL) == 0x0000000000000000ULL);

        b = 0x0000000000000001ULL;
        CHECK((b | 0x0000000000000001ULL) == 0x0000000000000001ULL);

        b = 0x0000000000000003ULL;
        CHECK((b | 0x0000000000000003ULL) == 0x0000000000000003ULL);
    }

    TEST_CASE("operator^") {
        Bitboard b = 0x0000000000000000ULL;

        CHECK((b ^ 0x0000000000000000ULL) == 0x0000000000000000ULL);

        b = 0x0000000000000001ULL;
        CHECK((b ^ 0x0000000000000001ULL) == 0x0000000000000000ULL);

        b = 0x0000000000000003ULL;
        CHECK((b ^ 0x0000000000000003ULL) == 0x0000000000000000ULL);
    }

    TEST_CASE("operator<<") {
        Bitboard b = 0x0000000000000000ULL;

        CHECK((b << 0) == 0x0000000000000000ULL);

        b = 0x0000000000000001ULL;
        CHECK((b << 1) == 0x0000000000000002ULL);

        b = 0x0000000000000003ULL;
        CHECK((b << 2) == 0x000000000000000cULL);
    }

    TEST_CASE("operator>>") {
        Bitboard b = 0x0000000000000000ULL;

        CHECK((b >> 0) == 0x0000000000000000ULL);

        b = 0x0000000000000001ULL;
        CHECK((b >> 1) == 0x0000000000000000ULL);

        b = 0x0000000000000003ULL;
        CHECK((b >> 2) == 0x0000000000000000ULL);
    }

    TEST_CASE("operator&=") {
        Bitboard b = 0x0000000000000000ULL;

        b &= 0x0000000000000000ULL;
        CHECK(b == 0x0000000000000000ULL);

        b = 0x0000000000000001ULL;
        b &= 0x0000000000000001ULL;
        CHECK(b == 0x0000000000000001ULL);

        b = 0x0000000000000003ULL;
        b &= 0x0000000000000003ULL;
        CHECK(b == 0x0000000000000003ULL);
    }

    TEST_CASE("operator|=") {
        Bitboard b = 0x0000000000000000ULL;

        b |= 0x0000000000000000ULL;
        CHECK(b == 0x0000000000000000ULL);

        b = 0x0000000000000001ULL;
        b |= 0x0000000000000001ULL;
        CHECK(b == 0x0000000000000001ULL);

        b = 0x0000000000000003ULL;
        b |= 0x0000000000000003ULL;
        CHECK(b == 0x0000000000000003ULL);
    }

    TEST_CASE("operator^=") {
        Bitboard b = 0x0000000000000000ULL;

        b ^= 0x0000000000000000ULL;
        CHECK(b == 0x0000000000000000ULL);

        b = 0x0000000000000001ULL;
        b ^= 0x0000000000000001ULL;
        CHECK(b == 0x0000000000000000ULL);

        b = 0x0000000000000003ULL;
        b ^= 0x0000000000000003ULL;
        CHECK(b == 0x0000000000000000ULL);
    }

    TEST_CASE("operator~") {
        Bitboard b = 0x0000000000000000ULL;

        CHECK(~b == 0xffffffffffffffffULL);

        b = 0x0000000000000001ULL;
        CHECK(~b == 0xfffffffffffffffeULL);

        b = 0x0000000000000003ULL;
        CHECK(~b == 0xfffffffffffffffcULL);
    }

    TEST_CASE("set") {
        Bitboard b = 0x0000000000000000ULL;

        b.set(0);
        CHECK(b == 0x0000000000000001ULL);

        b.set(1);
        CHECK(b == 0x0000000000000003ULL);

        b.set(2);
        CHECK(b == 0x0000000000000007ULL);
    }

    TEST_CASE("check") {
        Bitboard b = 0x0000000000000000ULL;

        CHECK(!b.check(0));

        b.set(0);
        CHECK(b.check(0));

        b.set(1);
        CHECK(b.check(1));
    }

    TEST_CASE("clear") {
        Bitboard b = 0x0000000000000000ULL;

        b.clear(0);
        CHECK(b == 0x0000000000000000ULL);

        b.set(0);
        b.clear(0);
        CHECK(b == 0x0000000000000000ULL);

        b.set(1);
        b.clear(1);
        CHECK(b == 0x0000000000000000ULL);
    }

    TEST_CASE("clearAll") {
        Bitboard b = 0x0000000000000000ULL;

        b.clear();
        CHECK(b == 0x0000000000000000ULL);

        b.set(0);
        b.clear();
        CHECK(b == 0x0000000000000000ULL);

        b.set(1);
        b.clear();
        CHECK(b == 0x0000000000000000ULL);
    }

    TEST_CASE("fromSquare") {
        Bitboard b = Bitboard::fromSquare(0);
        CHECK(b == 0x0000000000000001ULL);

        b = Bitboard::fromSquare(1);
        CHECK(b == 0x0000000000000002ULL);

        b = Bitboard::fromSquare(2);
        CHECK(b == 0x0000000000000004ULL);
    }

    TEST_CASE("operator string") {
        Bitboard b = Bitboard(0);

        const auto str = "00000000\n00000000\n00000000\n00000000\n00000000\n00000000\n00000000\n00000000\n";
        CHECK(static_cast<std::string>(str) == str);

        b               = Bitboard::fromSquare(Square::SQ_A1);
        const auto str1 = "00000000\n00000000\n00000000\n00000000\n00000000\n00000000\n00000000\n10000000\n";
        CHECK(static_cast<std::string>(str1) == str1);

        b               = Bitboard::fromSquare(Square::SQ_A2);
        const auto str2 = "00000000\n00000000\n00000000\n00000000\n00000000\n00000000\n00000000\n01000000\n";
        CHECK(static_cast<std::string>(str2) == str2);
    }
}