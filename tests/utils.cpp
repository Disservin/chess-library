#include "../src/include.hpp"
#include "doctest/doctest.hpp"

using namespace chess;

TEST_SUITE("Utilities") {
    TEST_CASE("extractSquare") {
        CHECK(Square("a1") == Square::underlying::SQ_A1);
        CHECK(Square("h8") == Square::underlying::SQ_H8);
    }

    TEST_CASE("fileRankSquare") {
        CHECK(Square(File::FILE_A, Rank::RANK_8) == Square::underlying::SQ_A8);
        CHECK(Square(File::FILE_H, Rank::RANK_1) == Square::underlying::SQ_H1);
    }

    TEST_CASE("squareRank") {
        CHECK(Square(Square::underlying::SQ_A1).rank() == Rank::RANK_1);
        CHECK(Square(Square::underlying::SQ_H8).rank() == Rank::RANK_8);
    }

    TEST_CASE("squareFile") {
        CHECK(Square(Square::underlying::SQ_A1).file() == File::FILE_A);
        CHECK(Square(Square::underlying::SQ_H8).file() == File::FILE_H);
    }

    TEST_CASE("squareDistance") {
        CHECK(Square(Square::underlying::SQ_A1).distance(Square::underlying::SQ_A2) == 1);
        CHECK(Square(Square::underlying::SQ_A1).distance(Square::underlying::SQ_A3) == 2);
    }

    TEST_CASE("typeOfPiece") {
        CHECK(Piece(Piece::WHITEPAWN).type() == PieceType::PAWN);
        CHECK(Piece(Piece::WHITEKING).type() == PieceType::KING);

        CHECK(Piece(Piece::BLACKPAWN).type() == PieceType::PAWN);
        CHECK(Piece(Piece::BLACKKING).type() == PieceType::KING);
    }

    TEST_CASE("makePiece") {
        CHECK(Piece(Color::WHITE, PieceType::PAWN) == Piece::WHITEPAWN);
        CHECK(Piece(Color::WHITE, PieceType::KING) == Piece::WHITEKING);
        CHECK(Piece(Color::BLACK, PieceType::PAWN) == Piece::BLACKPAWN);
        CHECK(Piece(Color::BLACK, PieceType::KING) == Piece::BLACKKING);
    }

    TEST_CASE("ourBackRank") {
        CHECK(utils::ourBackRank(Square::underlying::SQ_A1, Color::WHITE));
        CHECK(!utils::ourBackRank(Square::underlying::SQ_A2, Color::WHITE));

        CHECK(utils::ourBackRank(Square::underlying::SQ_A8, Color::BLACK));
        CHECK(!utils::ourBackRank(Square::underlying::SQ_A7, Color::BLACK));
    }
}