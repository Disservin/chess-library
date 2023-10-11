#include "../src/chess.hpp"
#include "doctest/doctest.hpp"

using namespace chess;

TEST_SUITE("Board Test") {
    TEST_CASE("makeMove") {
        Board board = Board();
        board.makeMove(Move::make(Square::SQ_E2, Square::SQ_E4));
        board.makeMove(Move::make(Square::SQ_E7, Square::SQ_E5));

        CHECK(board.at(Square::SQ_E4) == Piece::WHITEPAWN);
        CHECK(board.at(Square::SQ_E5) == Piece::BLACKPAWN);

        board.unmakeMove(Move::make(Square::SQ_E7, Square::SQ_E5));
        board.unmakeMove(Move::make(Square::SQ_E2, Square::SQ_E4));

        CHECK(board.at(Square::SQ_E2) == Piece::WHITEPAWN);
        CHECK(board.at(Square::SQ_E7) == Piece::BLACKPAWN);

        CHECK(board.zobrist() == Board().zobrist());
    }

    TEST_CASE("makeNullMove") {
        Board board = Board();
        board.makeNullMove();

        CHECK(board.zobrist() != Board().zobrist());
        CHECK(board.sideToMove() == Color::BLACK);

        board.unmakeNullMove();

        CHECK(board.zobrist() == Board().zobrist());
        CHECK(board.sideToMove() == Color::WHITE);
    }

    TEST_CASE("hasNonPawnMaterial") {
        Board board = Board("4k1n1/pppppppp/8/8/8/8/PPPPPPPP/4K3 w - - 0 1");
        CHECK(!board.hasNonPawnMaterial(board.sideToMove()));
        CHECK(!board.hasNonPawnMaterial(Color::WHITE));
        CHECK(board.hasNonPawnMaterial(Color::BLACK));
    }

    TEST_CASE("isHalfMoveDraw") {
        Board board = Board("4k1n1/pppppppp/8/8/8/8/PPPPPPPP/4K3 w - - 0 1");
        CHECK(board.isHalfMoveDraw() == false);
    }

    TEST_CASE("isHalfMoveDraw True") {
        Board board = Board("4k1n1/pppppppp/8/8/8/8/PPPPPPPP/4K3 w - - 100 1");
        CHECK(board.isHalfMoveDraw());
        CHECK(board.getHalfMoveDrawType().first == GameResultReason::FIFTY_MOVE_RULE);
    }

    TEST_CASE("isHalfMoveDraw True and Checkmate") {
        Board board = Board("7k/8/5B1K/8/8/1B6/8/8 b - - 100 1");
        CHECK(board.isHalfMoveDraw());
        CHECK(board.getHalfMoveDrawType().first == GameResultReason::CHECKMATE);
    }

    TEST_CASE("Test Fen Get/Set") {
        Board board = Board();
        board.setFen("4k1n1/pppppppp/8/8/8/8/PPPPPPPP/4K3 w - - 0 1");
        CHECK(board.getFen() == "4k1n1/pppppppp/8/8/8/8/PPPPPPPP/4K3 w - - 0 1");

        board.setFen("4k1n1/pppppppp/8/8/8/8/PPPPPPPP/4K3 w KQkq - 0 1");
        CHECK(board.getFen() == "4k1n1/pppppppp/8/8/8/8/PPPPPPPP/4K3 w KQkq - 0 1");

        board.setFen("4k1n1/pppppppp/8/8/8/8/PPPPPPPP/4K3 w KQkq e3 0 1");
        CHECK(board.getFen() == "4k1n1/pppppppp/8/8/8/8/PPPPPPPP/4K3 w KQkq e3 0 1");
    }
}