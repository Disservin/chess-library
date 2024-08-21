#include <map>

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

    TEST_CASE("Insufficient Material Two White Light Bishops") {
        Board board = Board("8/6k1/8/8/4B3/3B4/8/1K6 w - - 0 1");
        CHECK(board.isInsufficientMaterial());
    }

    TEST_CASE("Insufficient Material Two Black Light Bishops") {
        Board board = Board("8/6k1/8/8/4b3/3b4/K7/8 w - - 0 1");
        CHECK(board.isInsufficientMaterial());
    }

    TEST_CASE("Insufficient Material White Bishop") {
        Board board = Board("8/7k/8/8/3B4/8/8/1K6 w - - 0 1");
        CHECK(board.isInsufficientMaterial());
    }

    TEST_CASE("Insufficient Material Black Bishop") {
        Board board = Board("8/7k/8/8/3b4/8/8/1K6 w - - 0 1");
        CHECK(board.isInsufficientMaterial());
    }

    TEST_CASE("Insufficient Material White Knight") {
        Board board = Board("8/7k/8/8/3N4/8/8/1K6 w - - 0 1");
        CHECK(board.isInsufficientMaterial());
    }

    TEST_CASE("Insufficient Material Black Knight") {
        Board board = Board("8/7k/8/8/3n4/8/8/1K6 w - - 0 1");
        CHECK(board.isInsufficientMaterial());
    }

    TEST_CASE("Insufficient Material White Light Bishop and White Dark Bishop") {
        Board board = Board("8/7k/8/8/3BB3/8/8/1K6 w - - 0 1");
        CHECK(board.isInsufficientMaterial() == false);
    }

    TEST_CASE("Compressed State Normal") {
        Board board     = Board("4k1n1/pppppppp/8/8/8/8/PPPPPPPP/4K3 w - - 0 1");
        auto compressed = Board::Compressed::encode(board);
        auto newboard   = Board::Compressed::decode(compressed);

        CHECK(board.getFen() == newboard.getFen());
        CHECK(sizeof(compressed) == 24);
    }

    TEST_CASE("Compressed State EP ") {
        Board board     = Board("4k1n1/ppp1pppp/8/8/3pP3/8/PPPP1PPP/4K3 b - e3 0 1");
        auto compressed = Board::Compressed::encode(board);
        auto newboard   = Board::Compressed::decode(compressed);

        CHECK(board.getFen() == newboard.getFen());
    }

    TEST_CASE("Compressed State White Castling Queen") {
        Board board     = Board("4k1n1/pppppppp/8/8/8/8/PPPPPPPP/R3K3 w Q - 0 1");
        auto compressed = Board::Compressed::encode(board);
        auto newboard   = Board::Compressed::decode(compressed);

        CHECK(board.getFen() == newboard.getFen());
    }

    TEST_CASE("Compressed State White Castling King") {
        Board board     = Board("4k1n1/pppppppp/8/8/8/8/PPPPPPPP/4K2R w K - 0 1");
        auto compressed = Board::Compressed::encode(board);
        auto newboard   = Board::Compressed::decode(compressed);

        CHECK(board.getFen() == newboard.getFen());
    }

    TEST_CASE("Compressed State Black Castling Queen") {
        Board board     = Board("r3k1n1/pppppppp/8/8/8/8/PPPPPPPP/4K3 w q - 0 1");
        auto compressed = Board::Compressed::encode(board);
        auto newboard   = Board::Compressed::decode(compressed);

        CHECK(board.getFen() == newboard.getFen());
    }

    TEST_CASE("Compressed State Black Castling King") {
        Board board     = Board("4k1nr/pppppppp/8/8/8/8/PPPPPPPP/4K3 w k - 0 1");
        auto compressed = Board::Compressed::encode(board);
        auto newboard   = Board::Compressed::decode(compressed);

        CHECK(board.getFen() == newboard.getFen());
    }

    TEST_CASE("Compressed State Black Side to Move") {
        Board board     = Board("4k1n1/pppppppp/8/8/8/8/PPPPPPPP/4K3 b - - 0 1");
        auto compressed = Board::Compressed::encode(board);
        auto newboard   = Board::Compressed::decode(compressed);

        CHECK(board.getFen() == newboard.getFen());
    }

    TEST_CASE("Compressed State Usable in Map") {
        std::map<int, Board::Compressed> compressed;

        compressed.emplace(0, Board::Compressed::encode(Board("4k1n1/pppppppp/8/8/8/8/PPPPPPPP/4K3 w - - 0 1")));

        CHECK(Board::Compressed::decode(compressed.at(0)).getFen() == "4k1n1/pppppppp/8/8/8/8/PPPPPPPP/4K3 w - - 0 1");
    }
}