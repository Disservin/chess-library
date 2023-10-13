#include "../src/chess.hpp"
#include "doctest/doctest.hpp"

using namespace chess;

TEST_SUITE("Utilities") {
    TEST_CASE("extractSquare") {
        CHECK(utils::extractSquare("a1") == Square::SQ_A1);
        CHECK(utils::extractSquare("h8") == Square::SQ_H8);
    }

    TEST_CASE("fileRankSquare") {
        CHECK(utils::fileRankSquare(File::FILE_A, Rank::RANK_8) == Square::SQ_A8);
        CHECK(utils::fileRankSquare(File::FILE_H, Rank::RANK_1) == Square::SQ_H1);
    }

    TEST_CASE("squareRank") {
        CHECK(utils::squareRank(Square::SQ_A1) == Rank::RANK_1);
        CHECK(utils::squareRank(Square::SQ_H8) == Rank::RANK_8);
    }

    TEST_CASE("squareFile") {
        CHECK(utils::squareFile(Square::SQ_A1) == File::FILE_A);
        CHECK(utils::squareFile(Square::SQ_H8) == File::FILE_H);
    }

    TEST_CASE("squareDistance") {
        CHECK(utils::squareDistance(Square::SQ_A1, Square::SQ_A2) == 1);
        CHECK(utils::squareDistance(Square::SQ_A1, Square::SQ_A3) == 2);
    }

    TEST_CASE("typeOfPiece") {
        CHECK(utils::typeOfPiece(Piece::WHITEPAWN) == PieceType::PAWN);
        CHECK(utils::typeOfPiece(Piece::WHITEKING) == PieceType::KING);

        CHECK(utils::typeOfPiece(Piece::BLACKPAWN) == PieceType::PAWN);
        CHECK(utils::typeOfPiece(Piece::BLACKKING) == PieceType::KING);
    }

    TEST_CASE("makePiece") {
        CHECK(utils::makePiece(Color::WHITE, PieceType::PAWN) == Piece::WHITEPAWN);
        CHECK(utils::makePiece(Color::WHITE, PieceType::KING) == Piece::WHITEKING);
        CHECK(utils::makePiece(Color::BLACK, PieceType::PAWN) == Piece::BLACKPAWN);
        CHECK(utils::makePiece(Color::BLACK, PieceType::KING) == Piece::BLACKKING);
    }

    TEST_CASE("ourBackRank") {
        CHECK(utils::ourBackRank(Square::SQ_A1, Color::WHITE));
        CHECK(!utils::ourBackRank(Square::SQ_A2, Color::WHITE));

        CHECK(utils::ourBackRank(Square::SQ_A8, Color::BLACK));
        CHECK(!utils::ourBackRank(Square::SQ_A7, Color::BLACK));
    }
}