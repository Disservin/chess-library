#include "../src/include.hpp"
#include "doctest/doctest.hpp"

#include <sstream>

using namespace chess;

TEST_SUITE("Color") {
    TEST_CASE("operator~") {
        Color c = Color::WHITE;
        CHECK(~c == Color::BLACK);

        c = Color::BLACK;
        CHECK(~c == Color::WHITE);
    }

    TEST_CASE("operator==") {
        Color c = Color::WHITE;
        CHECK(c == Color::WHITE);

        c = Color::BLACK;
        CHECK(c == Color::BLACK);
    }

    TEST_CASE("operator!=") {
        Color c = Color::WHITE;
        CHECK(c != Color::BLACK);

        c = Color::BLACK;
        CHECK(c != Color::WHITE);
    }

    TEST_CASE("operator int") {
        Color c = Color::WHITE;
        CHECK(static_cast<int>(c) == 0);

        c = Color::BLACK;
        CHECK(static_cast<int>(c) == 1);
    }

    TEST_CASE("operator std::string") {
        Color c = Color::WHITE;
        CHECK(static_cast<std::string>(c) == "w");

        c = Color::BLACK;
        CHECK(static_cast<std::string>(c) == "b");
    }

    TEST_CASE("operator<<") {
        Color c = Color::WHITE;
        std::stringstream ss;
        ss << c;
        CHECK(ss.str() == "w");

        c = Color::BLACK;
        ss.str("");
        ss << c;
        CHECK(ss.str() == "b");
    }

    TEST_CASE("internal") {
        Color c = Color::WHITE;
        CHECK(c.internal() == Color::WHITE);

        c = Color::BLACK;
        CHECK(c.internal() == Color::BLACK);
    }

    TEST_CASE("fromString") {
        Color c = Color("w");
        CHECK(c == Color::WHITE);

        c = Color("b");
        CHECK(c == Color::BLACK);
    }
}