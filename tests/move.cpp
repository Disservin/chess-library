#include "../src/include.hpp"
#include "doctest/doctest.hpp"

using namespace chess;

TEST_SUITE("Move") {
    TEST_CASE("from") {
        CHECK(Move::make(Square::underlying::SQ_A1, Square::underlying::SQ_A2).from() ==
              Square::underlying::SQ_A1);
        CHECK(Move::make(Square::underlying::SQ_H8, Square::underlying::SQ_H1).from() ==
              Square::underlying::SQ_H8);
    }

    TEST_CASE("to") {
        CHECK(Move::make(Square::underlying::SQ_A1, Square::underlying::SQ_A2).to() ==
              Square::underlying::SQ_A2);
        CHECK(Move::make(Square::underlying::SQ_H8, Square::underlying::SQ_H1).to() ==
              Square::underlying::SQ_H1);
    }

    TEST_CASE("typeOf") {
        CHECK(Move::make(Square::underlying::SQ_A1, Square::underlying::SQ_A2).typeOf() ==
              Move::NORMAL);
        CHECK(Move::make<Move::PROMOTION>(Square::underlying::SQ_H7, Square::underlying::SQ_H8)
                  .typeOf() == Move::PROMOTION);
        CHECK(Move::make<Move::ENPASSANT>(Square::underlying::SQ_D5, Square::underlying::SQ_C6)
                  .typeOf() == Move::ENPASSANT);
        CHECK(Move::make<Move::CASTLING>(Square::underlying::SQ_E8, Square::underlying::SQ_H8)
                  .typeOf() == Move::CASTLING);
    }

    TEST_CASE("promotionType") {
        CHECK(Move::make<Move::PROMOTION>(Square::underlying::SQ_A1, Square::underlying::SQ_A2,
                                          PieceType::BISHOP)
                  .promotionType() == PieceType::BISHOP);
        CHECK(Move::make<Move::PROMOTION>(Square::underlying::SQ_H7, Square::underlying::SQ_H8,
                                          PieceType::KNIGHT)
                  .promotionType() == PieceType::KNIGHT);
        CHECK(Move::make<Move::PROMOTION>(Square::underlying::SQ_D5, Square::underlying::SQ_C6,
                                          PieceType::ROOK)
                  .promotionType() == PieceType::ROOK);
        CHECK(Move::make<Move::PROMOTION>(Square::underlying::SQ_E8, Square::underlying::SQ_H8,
                                          PieceType::QUEEN)
                  .promotionType() == PieceType::QUEEN);
    }
}