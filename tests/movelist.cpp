#include "../src/include.hpp"
#include "doctest/doctest.hpp"

using namespace chess;

TEST_SUITE("Movelist") {
    TEST_CASE("add") {
        Movelist moves;
        moves.add(Move::make(Square::underlying::SQ_A1, Square::underlying::SQ_A2));

        CHECK(moves.size() == 1);
        CHECK(moves[0] == Move::make(Square::underlying::SQ_A1, Square::underlying::SQ_A2));
        CHECK(moves.empty() == false);
    }

    TEST_CASE("find") {
        Movelist moves;
        moves.add(Move::make(Square::underlying::SQ_A1, Square::underlying::SQ_A2));
        moves.add(Move::make(Square::underlying::SQ_A1, Square::underlying::SQ_A3));
        moves.add(Move::make(Square::underlying::SQ_A1, Square::underlying::SQ_A4));

        CHECK(std::find(moves.begin(), moves.end(), Move::make(Square::underlying::SQ_A1, Square::underlying::SQ_A2)) !=
              moves.end());
        CHECK(std::find(moves.begin(), moves.end(), Move::make(Square::underlying::SQ_A1, Square::underlying::SQ_A3)) !=
              moves.end());
        CHECK(std::find(moves.begin(), moves.end(), Move::make(Square::underlying::SQ_A1, Square::underlying::SQ_A4)) !=
              moves.end());
        CHECK(std::find(moves.begin(), moves.end(), Move::make(Square::underlying::SQ_A1, Square::underlying::SQ_A5)) ==
              moves.end());
    }
}
