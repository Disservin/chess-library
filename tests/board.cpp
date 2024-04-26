#include "../src/include.hpp"
#include "doctest/doctest.hpp"

using namespace chess;

TEST_SUITE("Board") {
    TEST_CASE("makeMove") {
        Board board = Board();
        board.makeMove(Move::make(Square::underlying::SQ_E2, Square::underlying::SQ_E4));
        board.makeMove(Move::make(Square::underlying::SQ_E7, Square::underlying::SQ_E5));

        CHECK(board.at(Square::underlying::SQ_E4) == Piece::WHITEPAWN);
        CHECK(board.at(Square::underlying::SQ_E5) == Piece::BLACKPAWN);

        board.unmakeMove(Move::make(Square::underlying::SQ_E7, Square::underlying::SQ_E5));
        board.unmakeMove(Move::make(Square::underlying::SQ_E2, Square::underlying::SQ_E4));

        CHECK(board.at(Square::underlying::SQ_E2) == Piece::WHITEPAWN);
        CHECK(board.at(Square::underlying::SQ_E7) == Piece::BLACKPAWN);

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

        board.setFen("4k1n1/pppppppp/8/8/8/8/PPPPPPPP/4K3 w - - 3 24");
        CHECK(board.getFen() == "4k1n1/pppppppp/8/8/8/8/PPPPPPPP/4K3 w - - 3 24");

        board.setFen("4k1n1/pppppppp/8/8/8/8/PPPPPPPP/4K3 w KQkq - 0 1");
        CHECK(board.getFen() == "4k1n1/pppppppp/8/8/8/8/PPPPPPPP/4K3 w KQkq - 0 1");

        board.setFen("4k1n1/pppppppp/8/8/8/8/PPPPPPPP/4K3 w KQkq e3 0 1");
        CHECK(board.getFen() == "4k1n1/pppppppp/8/8/8/8/PPPPPPPP/4K3 w KQkq e3 0 1");

        board.setFen("4k1n1/pppppppp/8/8/8/8/PPPPPPPP/4K3 w KQkq e3 0 1");
        CHECK(board.getFen(false) == "4k1n1/pppppppp/8/8/8/8/PPPPPPPP/4K3 w KQkq e3");

        board.setFen("4k1n1/pppppppp/8/8/8/8/PPPPPPPP/4K3 w - - 3 24");
        CHECK(board.getFen(false) == "4k1n1/pppppppp/8/8/8/8/PPPPPPPP/4K3 w - -");

        board.setFen("r1bqkb1r/1ppp1ppp/p1n2n2/4p3/B3P3/5N2/PPPP1PPP/RNBQK2R w KQkq -");
        CHECK(board.getFen() == "r1bqkb1r/1ppp1ppp/p1n2n2/4p3/B3P3/5N2/PPPP1PPP/RNBQK2R w KQkq - 0 1");
    }

    TEST_CASE("Set EPD") {
        Board board = Board();
        board.setEpd("r1bqkb1r/pp3pp1/2nppn2/7p/3NP1PP/2N5/PPP2P2/R1BQKBR1 w Qkq - hmvc 0; fmvn 9;");
        CHECK(board.getFen() == "r1bqkb1r/pp3pp1/2nppn2/7p/3NP1PP/2N5/PPP2P2/R1BQKBR1 w Qkq - 0 9");
        CHECK(board.getEpd() == "r1bqkb1r/pp3pp1/2nppn2/7p/3NP1PP/2N5/PPP2P2/R1BQKBR1 w Qkq - hmvc 0; fmvn 9;");
    }

    TEST_CASE("From Fen") {
        Board board = Board::fromFen("4k1n1/pppppppp/8/8/8/8/PPPPPPPP/4K3 w - - 0 1");
        CHECK(board.getFen() == "4k1n1/pppppppp/8/8/8/8/PPPPPPPP/4K3 w - - 0 1");
    }

    TEST_CASE("From EPD") {
        Board board = Board::fromEpd("r1bqkb1r/pp3pp1/2nppn2/7p/3NP1PP/2N5/PPP2P2/R1BQKBR1 w Qkq - hmvc 0; fmvn 9;");
        CHECK(board.getFen() == "r1bqkb1r/pp3pp1/2nppn2/7p/3NP1PP/2N5/PPP2P2/R1BQKBR1 w Qkq - 0 9");
        CHECK(board.getEpd() == "r1bqkb1r/pp3pp1/2nppn2/7p/3NP1PP/2N5/PPP2P2/R1BQKBR1 w Qkq - hmvc 0; fmvn 9;");
    }

    TEST_CASE("From EPD with hmvc") {
        Board board =
            Board::fromEpd("r1bqk1nr/1p1p1ppp/p1n1pb2/8/4P3/1N1B2Q1/PPP2PPP/RNB1K2R w KQkq - hmvc 8; fmvn 9;");
        CHECK(board.getFen() == "r1bqk1nr/1p1p1ppp/p1n1pb2/8/4P3/1N1B2Q1/PPP2PPP/RNB1K2R w KQkq - 8 9");
        CHECK(board.getEpd() == "r1bqk1nr/1p1p1ppp/p1n1pb2/8/4P3/1N1B2Q1/PPP2PPP/RNB1K2R w KQkq - hmvc 8; fmvn 9;");
    }
}