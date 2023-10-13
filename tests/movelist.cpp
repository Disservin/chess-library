#include "../src/chess.hpp"
#include "doctest/doctest.hpp"

using namespace chess;

TEST_SUITE("Movelist") {
    TEST_CASE("add") {
        Movelist moves;
        moves.add(Move::make(Square::SQ_A1, Square::SQ_A2));

        CHECK(moves.size() == 1);
        CHECK(moves[0] == Move::make(Square::SQ_A1, Square::SQ_A2));
        CHECK(moves.empty() == false);
    }

    TEST_CASE("find") {
        Movelist moves;
        moves.add(Move::make(Square::SQ_A1, Square::SQ_A2));
        moves.add(Move::make(Square::SQ_A1, Square::SQ_A3));
        moves.add(Move::make(Square::SQ_A1, Square::SQ_A4));

        CHECK(moves.find(Move::make(Square::SQ_A1, Square::SQ_A2)) == 0);
        CHECK(moves.find(Move::make(Square::SQ_A1, Square::SQ_A3)) == 1);
        CHECK(moves.find(Move::make(Square::SQ_A1, Square::SQ_A4)) == 2);
        CHECK(moves.find(Move::make(Square::SQ_A1, Square::SQ_A5)) == -1);
    }
}