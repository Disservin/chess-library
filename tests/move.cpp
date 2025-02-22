#include "../src/include.hpp"
#include "doctest/doctest.hpp"

using namespace chess;

TEST_SUITE("Move") {
    TEST_CASE("from") {
        CHECK(Move::make(Square::SQ_A1, Square::SQ_A2).from() == Square::SQ_A1);
        CHECK(Move::make(Square::SQ_H8, Square::SQ_H1).from() == Square::SQ_H8);
    }

    TEST_CASE("to") {
        CHECK(Move::make(Square::SQ_A1, Square::SQ_A2).to() == Square::SQ_A2);
        CHECK(Move::make(Square::SQ_H8, Square::SQ_H1).to() == Square::SQ_H1);
    }

    TEST_CASE("typeOf") {
        CHECK(Move::make(Square::SQ_A1, Square::SQ_A2).typeOf() == Move::NORMAL);
        CHECK(Move::make<Move::PROMOTION>(Square::SQ_H7, Square::SQ_H8).typeOf() == Move::PROMOTION);
        CHECK(Move::make<Move::ENPASSANT>(Square::SQ_D5, Square::SQ_C6).typeOf() == Move::ENPASSANT);
        CHECK(Move::make<Move::CASTLING>(Square::SQ_E8, Square::SQ_H8).typeOf() == Move::CASTLING);
    }

    TEST_CASE("promotionType") {
        CHECK(Move::make<Move::PROMOTION>(Square::SQ_A1, Square::SQ_A2, PieceType::BISHOP).promotionType() ==
              PieceType::BISHOP);
        CHECK(Move::make<Move::PROMOTION>(Square::SQ_H7, Square::SQ_H8, PieceType::KNIGHT).promotionType() ==
              PieceType::KNIGHT);
        CHECK(Move::make<Move::PROMOTION>(Square::SQ_D5, Square::SQ_C6, PieceType::ROOK).promotionType() ==
              PieceType::ROOK);
        CHECK(Move::make<Move::PROMOTION>(Square::SQ_E8, Square::SQ_H8, PieceType::QUEEN).promotionType() ==
              PieceType::QUEEN);
    }
}