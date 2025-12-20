#include <map>

#include "../src/include.hpp"
#include "doctest/doctest.hpp"

using namespace chess;

TEST_SUITE("Board") {
    TEST_CASE("Set Fen") {
        Board board;
        CHECK(board.setFen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - "));
        CHECK(board.getFen() == "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    }

    TEST_CASE("Set xFEN") {
        Board board;
        CHECK(board.setXfen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"));
        CHECK(board.chess960());
        CHECK(board.castlingRights().getRookFile(Color::WHITE, Board::CastlingRights::Side::KING_SIDE) == File::FILE_H);
        CHECK(board.castlingRights().getRookFile(Color::WHITE, Board::CastlingRights::Side::QUEEN_SIDE) ==
              File::FILE_A);
        CHECK(board.castlingRights().getRookFile(Color::BLACK, Board::CastlingRights::Side::KING_SIDE) == File::FILE_H);
        CHECK(board.castlingRights().getRookFile(Color::BLACK, Board::CastlingRights::Side::QUEEN_SIDE) ==
              File::FILE_A);
        CHECK(board.getXfen() == "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    }

    TEST_CASE("xFen Lichess Example") {
        CHECK(Board::fromXfen("rnbnkqrb/pppppppp/8/8/8/8/PPPPPPPP/RNBNKQRB w KQkq - 0 1").getXfen() ==
              "rnbnkqrb/pppppppp/8/8/8/8/PPPPPPPP/RNBNKQRB w KQkq - 0 1");
        CHECK(Board::fromXfen("rnb1k1r1/ppp1pp1p/3p2p1/6n1/P7/2N2B2/1PPPPP2/2BNK1RR b Gkq - 3 10").getXfen() ==
              "rnb1k1r1/ppp1pp1p/3p2p1/6n1/P7/2N2B2/1PPPPP2/2BNK1RR b Gkq - 3 10");
    }

    TEST_CASE("xFEN castling uses outermost rook") {
        Board board;
        CHECK(board.setXfen("4k3/8/8/8/8/8/8/4KR1R w K - 0 1"));
        CHECK(board.castlingRights().getRookFile(Color::WHITE, Board::CastlingRights::Side::KING_SIDE) == File::FILE_H);
        CHECK(board.getXfen() == "4k3/8/8/8/8/8/8/4KR1R w K - 0 1");
    }

    TEST_CASE("xFEN castling file letter selects rook") {
        Board board;
        CHECK(board.setXfen("4k3/8/8/8/8/8/8/4KR1R w F - 0 1"));
        CHECK(board.castlingRights().getRookFile(Color::WHITE, Board::CastlingRights::Side::KING_SIDE) == File::FILE_F);
        CHECK(board.getXfen() == "4k3/8/8/8/8/8/8/4KR1R w F - 0 1");
    }

    TEST_CASE("xFEN castling file letter lowercase for black") {
        Board board;
        CHECK(board.setXfen("4kr1r/8/8/8/8/8/8/4K3 b f - 0 1"));
        CHECK(board.castlingRights().getRookFile(Color::BLACK, Board::CastlingRights::Side::KING_SIDE) == File::FILE_F);
        CHECK(board.getXfen() == "4kr1r/8/8/8/8/8/8/4K3 b f - 0 1");
    }

    TEST_CASE("xFen with Makemove") {
        auto board = Board("rnb1k1r1/ppp1pp1p/3p2p1/6n1/P7/2N2B1R/1PPPPP2/2BNK1R1 w Kkq - 2 10");

        board.makeMove(uci::parseSan(board, "Rhh1"));

        CHECK(board.getXfen() == "rnb1k1r1/ppp1pp1p/3p2p1/6n1/P7/2N2B2/1PPPPP2/2BNK1RR b Gkq - 3 10");
    }

    TEST_CASE("Board makeMove/unmakeMove") {
        SUBCASE("makeMove") {
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

        SUBCASE("makeNullMove") {
            Board board = Board();
            board.makeNullMove();

            CHECK(board.zobrist() != Board().zobrist());
            CHECK(board.sideToMove() == Color::BLACK);

            board.unmakeNullMove();

            CHECK(board.zobrist() == Board().zobrist());
            CHECK(board.sideToMove() == Color::WHITE);
        }
    }

    TEST_CASE("Board ") {
        SUBCASE("hasNonPawnMaterial") {
            Board board = Board("4k1n1/pppppppp/8/8/8/8/PPPPPPPP/4K3 w - - 0 1");
            CHECK(!board.hasNonPawnMaterial(board.sideToMove()));
            CHECK(!board.hasNonPawnMaterial(Color::WHITE));
            CHECK(board.hasNonPawnMaterial(Color::BLACK));
        }
    }

    TEST_CASE("Board HalfMove Draw") {
        SUBCASE("isHalfMoveDraw") {
            Board board = Board("4k1n1/pppppppp/8/8/8/8/PPPPPPPP/4K3 w - - 0 1");
            CHECK(board.isHalfMoveDraw() == false);
        }

        SUBCASE("isHalfMoveDraw True") {
            Board board = Board("4k1n1/pppppppp/8/8/8/8/PPPPPPPP/4K3 w - - 100 1");
            CHECK(board.isHalfMoveDraw());
            CHECK(board.getHalfMoveDrawType().first == GameResultReason::FIFTY_MOVE_RULE);
        }

        SUBCASE("isHalfMoveDraw True and Checkmate") {
            Board board = Board("7k/8/5B1K/8/8/1B6/8/8 b - - 100 1");
            CHECK(board.isHalfMoveDraw());
            CHECK(board.getHalfMoveDrawType().first == GameResultReason::CHECKMATE);
        }
    }

    TEST_CASE("Board Fen/EPD Interface") {
        SUBCASE("Test Fen Get/Set") {
            Board board = Board();
            board.setFen("4k1n1/pppppppp/8/8/8/8/PPPPPPPP/4K3 w - - 0 1");
            CHECK(board.getFen() == "4k1n1/pppppppp/8/8/8/8/PPPPPPPP/4K3 w - - 0 1");

            board.setFen("4k1n1/pppppppp/8/8/8/8/PPPPPPPP/4K3 w - - 3 24");
            CHECK(board.getFen() == "4k1n1/pppppppp/8/8/8/8/PPPPPPPP/4K3 w - - 3 24");

            board.setFen("4k1n1/pppppppp/8/8/8/8/PPPPPPPP/4K3 w KQkq - 0 1");
            CHECK(board.getFen() == "4k1n1/pppppppp/8/8/8/8/PPPPPPPP/4K3 w - - 0 1");

            board.setFen("4k1n1/pppppppp/8/8/8/8/PPPPPPPP/4K3 w KQkq e3 0 1");
            CHECK(board.getFen() == "4k1n1/pppppppp/8/8/8/8/PPPPPPPP/4K3 w - - 0 1");

            board.setFen("4k1n1/pppppppp/8/8/8/8/PPPPPPPP/4K3 w KQkq e3 0 1");
            CHECK(board.getFen(false) == "4k1n1/pppppppp/8/8/8/8/PPPPPPPP/4K3 w - -");

            board.setFen("4k1n1/pppppppp/8/8/8/8/PPPPPPPP/4K3 w - - 3 24");
            CHECK(board.getFen(false) == "4k1n1/pppppppp/8/8/8/8/PPPPPPPP/4K3 w - -");

            board.setFen("r1bqkb1r/1ppp1ppp/p1n2n2/4p3/B3P3/5N2/PPPP1PPP/RNBQK2R w KQkq -");
            CHECK(board.getFen() == "r1bqkb1r/1ppp1ppp/p1n2n2/4p3/B3P3/5N2/PPPP1PPP/RNBQK2R w KQkq - 0 1");
        }

        SUBCASE("Set EPD") {
            Board board = Board();
            board.setEpd("r1bqkb1r/pp3pp1/2nppn2/7p/3NP1PP/2N5/PPP2P2/R1BQKBR1 w Qkq - hmvc 0; fmvn 9;");
            CHECK(board.getFen() == "r1bqkb1r/pp3pp1/2nppn2/7p/3NP1PP/2N5/PPP2P2/R1BQKBR1 w Qkq - 0 9");
            CHECK(board.getEpd() == "r1bqkb1r/pp3pp1/2nppn2/7p/3NP1PP/2N5/PPP2P2/R1BQKBR1 w Qkq - hmvc 0; fmvn 9;");
        }

        SUBCASE("Set EPD Without FMVN") {
            Board board = Board();
            board.setEpd("r1bqkb1r/pp3pp1/2nppn2/7p/3NP1PP/2N5/PPP2P2/R1BQKBR1 w Qkq - hmvc 0;");
            CHECK(board.getFen() == "r1bqkb1r/pp3pp1/2nppn2/7p/3NP1PP/2N5/PPP2P2/R1BQKBR1 w Qkq - 0 1");
            CHECK(board.getEpd() == "r1bqkb1r/pp3pp1/2nppn2/7p/3NP1PP/2N5/PPP2P2/R1BQKBR1 w Qkq - hmvc 0; fmvn 1;");
        }

        SUBCASE("Set EPD Without HMVC") {
            Board board = Board();
            board.setEpd("r1bqkb1r/pp3pp1/2nppn2/7p/3NP1PP/2N5/PPP2P2/R1BQKBR1 w Qkq - fmvn 9;");
            CHECK(board.getFen() == "r1bqkb1r/pp3pp1/2nppn2/7p/3NP1PP/2N5/PPP2P2/R1BQKBR1 w Qkq - 0 9");
            CHECK(board.getEpd() == "r1bqkb1r/pp3pp1/2nppn2/7p/3NP1PP/2N5/PPP2P2/R1BQKBR1 w Qkq - hmvc 0; fmvn 9;");
        }

        SUBCASE("Set EPD Without HMVC & FMVN") {
            Board board = Board();
            board.setEpd("r1bqkb1r/pp3pp1/2nppn2/7p/3NP1PP/2N5/PPP2P2/R1BQKBR1 w Qkq -");
            CHECK(board.getFen() == "r1bqkb1r/pp3pp1/2nppn2/7p/3NP1PP/2N5/PPP2P2/R1BQKBR1 w Qkq - 0 1");
            CHECK(board.getEpd() == "r1bqkb1r/pp3pp1/2nppn2/7p/3NP1PP/2N5/PPP2P2/R1BQKBR1 w Qkq - hmvc 0; fmvn 1;");
        }

        SUBCASE("From Fen") {
            Board board = Board::fromFen("4k1n1/pppppppp/8/8/8/8/PPPPPPPP/4K3 w - - 0 1");
            CHECK(board.getFen() == "4k1n1/pppppppp/8/8/8/8/PPPPPPPP/4K3 w - - 0 1");
        }

        SUBCASE("From EPD") {
            Board board =
                Board::fromEpd("r1bqkb1r/pp3pp1/2nppn2/7p/3NP1PP/2N5/PPP2P2/R1BQKBR1 w Qkq - hmvc 0; fmvn 9;");
            CHECK(board.getFen() == "r1bqkb1r/pp3pp1/2nppn2/7p/3NP1PP/2N5/PPP2P2/R1BQKBR1 w Qkq - 0 9");
            CHECK(board.getEpd() == "r1bqkb1r/pp3pp1/2nppn2/7p/3NP1PP/2N5/PPP2P2/R1BQKBR1 w Qkq - hmvc 0; fmvn 9;");
        }

        SUBCASE("From EPD with hmvc") {
            Board board =
                Board::fromEpd("r1bqk1nr/1p1p1ppp/p1n1pb2/8/4P3/1N1B2Q1/PPP2PPP/RNB1K2R w KQkq - hmvc 8; fmvn 9;");
            CHECK(board.getFen() == "r1bqk1nr/1p1p1ppp/p1n1pb2/8/4P3/1N1B2Q1/PPP2PPP/RNB1K2R w KQkq - 8 9");
            CHECK(board.getEpd() == "r1bqk1nr/1p1p1ppp/p1n1pb2/8/4P3/1N1B2Q1/PPP2PPP/RNB1K2R w KQkq - hmvc 8; fmvn 9;");
        }

        SUBCASE("From Fen With Invalid Ep") {
            Board board = Board::fromFen("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1");
            CHECK(board.getFen() == "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq - 0 1");
        }
    }
    TEST_CASE("Board Insufficient Material") {
        SUBCASE("Insufficient Material Two White Light Bishops") {
            Board board = Board("8/6k1/8/8/4B3/3B4/8/1K6 w - - 0 1");
            CHECK(board.isInsufficientMaterial());
        }

        SUBCASE("Insufficient Material Two Black Light Bishops") {
            Board board = Board("8/6k1/8/8/4b3/3b4/K7/8 w - - 0 1");
            CHECK(board.isInsufficientMaterial());
        }

        SUBCASE("Insufficient Material White Bishop") {
            Board board = Board("8/7k/8/8/3B4/8/8/1K6 w - - 0 1");
            CHECK(board.isInsufficientMaterial());
        }

        SUBCASE("Insufficient Material Black Bishop") {
            Board board = Board("8/7k/8/8/3b4/8/8/1K6 w - - 0 1");
            CHECK(board.isInsufficientMaterial());
        }

        SUBCASE("Insufficient Material White Knight") {
            Board board = Board("8/7k/8/8/3N4/8/8/1K6 w - - 0 1");
            CHECK(board.isInsufficientMaterial());
        }

        SUBCASE("Insufficient Material Black Knight") {
            Board board = Board("8/7k/8/8/3n4/8/8/1K6 w - - 0 1");
            CHECK(board.isInsufficientMaterial());
        }

        SUBCASE("Insufficient Material White Light Bishop and White Dark Bishop") {
            Board board = Board("8/7k/8/8/3BB3/8/8/1K6 w - - 0 1");
            CHECK(board.isInsufficientMaterial() == false);
        }
    }

    TEST_CASE("PackedBoard") {
        SUBCASE("encode and decode ") {
            Board board     = Board("4k1n1/pppppppp/8/8/8/8/PPPPPPPP/4K3 w - - 0 1");
            auto compressed = Board::Compact::encode(board);
            auto newboard   = Board::Compact::decode(compressed);

            CHECK(board == newboard);
            CHECK(sizeof(compressed) == 24);
        }

        SUBCASE("same zobrist hash") {
            Board board     = Board("4k1n1/pppppppp/8/8/8/8/PPPPPPPP/4K3 w - - 0 1");
            auto compressed = Board::Compact::encode(board);
            auto newboard   = Board::Compact::decode(compressed);

            CHECK(board.hash() == newboard.hash());
        }

        SUBCASE("encode and decode with ep square") {
            Board board     = Board("4k1n1/ppp1pppp/8/8/3pP3/8/PPPP1PPP/4K3 b - e3 0 1");
            auto compressed = Board::Compact::encode(board);
            auto newboard   = Board::Compact::decode(compressed);

            CHECK(board == newboard);
        }

        SUBCASE("encode and decode with white to move") {
            Board board     = Board("4k1n1/ppp1p1pp/8/4Pp2/3p4/8/PPPP1PPP/4K3 w - f6 0 1");
            auto compressed = Board::Compact::encode(board);
            auto newboard   = Board::Compact::decode(compressed);

            CHECK(board == newboard);
        }

        SUBCASE("encode and decode with white castling bug") {
            Board board     = Board("rnb1kbnR/pppp4/5q2/4pp2/8/8/PPPPPP1P/RNBQKBNR b KQq - 0 1");
            auto compressed = Board::Compact::encode(board);
            auto newboard   = Board::Compact::decode(compressed);

            CHECK(board == newboard);
        }

        SUBCASE("encode and decode with white castling queen") {
            Board board     = Board("4k1n1/pppppppp/8/8/8/8/PPPPPPPP/R3K3 w Q - 0 1");
            auto compressed = Board::Compact::encode(board);
            auto newboard   = Board::Compact::decode(compressed);

            CHECK(board == newboard);
        }

        SUBCASE("encode and decode with white castling king") {
            Board board     = Board("4k1n1/pppppppp/8/8/8/8/PPPPPPPP/4K2R w K - 0 1");
            auto compressed = Board::Compact::encode(board);
            auto newboard   = Board::Compact::decode(compressed);

            CHECK(board == newboard);
        }

        SUBCASE("encode and decode with black castling queen") {
            Board board     = Board("r3k1n1/pppppppp/8/8/8/8/PPPPPPPP/4K3 w q - 0 1");
            auto compressed = Board::Compact::encode(board);
            auto newboard   = Board::Compact::decode(compressed);

            CHECK(board == newboard);
        }

        SUBCASE("encode and decode with black castling king") {
            Board board     = Board("4k1nr/pppppppp/8/8/8/8/PPPPPPPP/4K3 w k - 0 1");
            auto compressed = Board::Compact::encode(board);
            auto newboard   = Board::Compact::decode(compressed);

            CHECK(board == newboard);
        }

        SUBCASE("encode and decode with black side to move") {
            Board board     = Board("4k1n1/pppppppp/8/8/8/8/PPPPPPPP/4K3 b - - 0 1");
            auto compressed = Board::Compact::encode(board);
            auto newboard   = Board::Compact::decode(compressed);

            CHECK(board == newboard);
        }

        SUBCASE("usable in Map") {
            std::map<int, chess::PackedBoard> compressed;

            compressed.emplace(0, Board::Compact::encode(Board("4k1n1/pppppppp/8/8/8/8/PPPPPPPP/4K3 w - - 0 1")));

            CHECK(Board::Compact::decode(compressed.at(0)).getFen() == "4k1n1/pppppppp/8/8/8/8/PPPPPPPP/4K3 w - - 0 1");
        }
    }

    TEST_CASE("PackedBoard Chess960") {
        SUBCASE("castling") {
            Board board = Board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w AHah - 0 1", true);

            auto compressed = Board::Compact::encode(board);
            auto newboard   = Board::Compact::decode(compressed, true);

            CHECK(board == newboard);
        }

        SUBCASE("castling 2") {
            Board board = Board("1rqbkrbn/1ppppp1p/1n6/p1N3p1/8/2P4P/PP1PPPP1/1RQBKRBN w FBfb - 0 1", true);

            auto compressed = Board::Compact::encode(board);
            auto newboard   = Board::Compact::decode(compressed, true);

            CHECK(board == newboard);
        }

        SUBCASE("castling 3") {
            Board board = Board("rbbqn1kr/pp2p1pp/6n1/2pp1p2/2P4P/P7/BP1PPPP1/R1BQNNKR w HAha - 0 1", true);

            auto compressed = Board::Compact::encode(board);
            auto newboard   = Board::Compact::decode(compressed, true);

            CHECK(board == newboard);
        }

        SUBCASE("castling 4") {
            Board board = Board("rqbbknr1/1ppp2pp/p5n1/4pp2/P7/1PP5/1Q1PPPPP/R1BBKNRN w GAga - 0 1", true);

            auto compressed = Board::Compact::encode(board);
            auto newboard   = Board::Compact::decode(compressed, true);

            CHECK(board == newboard);
        }

        SUBCASE("castling 5") {
            Board board = Board("4rrb1/1kp3b1/1p1p4/pP1Pn2p/5p2/1PR2P2/2P1NB1P/2KR1B2 w D - 0 1", true);

            auto compressed = Board::Compact::encode(board);
            auto newboard   = Board::Compact::decode(compressed, true);

            CHECK(board == newboard);
        }

        SUBCASE("castling 6") {
            Board board = Board("1rkr3b/1ppn3p/3pB1n1/6q1/R2P4/4N1P1/1P5P/2KRQ1B1 b Dbd - 0 1", true);

            auto compressed = Board::Compact::encode(board);
            auto newboard   = Board::Compact::decode(compressed, true);

            CHECK(board == newboard);
        }

        SUBCASE("castling 7") {
            Board board = Board("qbbnrkr1/p1pppppp/1p4n1/8/2P5/6N1/PPNPPPPP/1BRKBRQ1 b FCge - 0 1", true);

            auto compressed = Board::Compact::encode(board);
            auto newboard   = Board::Compact::decode(compressed, true);

            CHECK(board == newboard);
        }

        SUBCASE("castling 8") {
            Board board = Board("rr6/2kpp3/1ppn2p1/p2b1q1p/P4P1P/1PNN2P1/2PP4/1K2R2R b E - 0 1", true);

            auto compressed = Board::Compact::encode(board);
            auto newboard   = Board::Compact::decode(compressed, true);

            CHECK(board == newboard);
        }

        SUBCASE("castling 9") {
            Board board = Board("rr6/2kpp3/1ppnb1p1/p2Q1q1p/P4P1P/1PNN2P1/2PP4/1K2RR2 b E - 0 1", true);

            auto compressed = Board::Compact::encode(board);
            auto newboard   = Board::Compact::decode(compressed, true);

            CHECK(board == newboard);
        }
    }

    TEST_CASE("PackedBoard from FEN") {
        SUBCASE("encode and decode") {
            auto compressed = Board::Compact::encode("4k1n1/pppppppp/8/8/8/8/PPPPPPPP/4K3 w - - 0 1");
            auto newboard   = Board::Compact::decode(compressed);

            CHECK("4k1n1/pppppppp/8/8/8/8/PPPPPPPP/4K3 w - - 0 1" == newboard.getFen());
            CHECK(sizeof(compressed) == 24);
        }

        SUBCASE("encode and decode with ep square") {
            auto compressed = Board::Compact::encode("4k1n1/ppp1pppp/8/8/3pP3/8/PPPP1PPP/4K3 b - e3 0 1");
            auto newboard   = Board::Compact::decode(compressed);

            CHECK("4k1n1/ppp1pppp/8/8/3pP3/8/PPPP1PPP/4K3 b - e3 0 1" == newboard.getFen());
        }

        SUBCASE("encode and decode with white to move") {
            auto compressed = Board::Compact::encode("4k1n1/ppp1p1pp/8/4Pp2/3p4/8/PPPP1PPP/4K3 w - f6 0 1");
            auto newboard   = Board::Compact::decode(compressed);

            CHECK("4k1n1/ppp1p1pp/8/4Pp2/3p4/8/PPPP1PPP/4K3 w - f6 0 1" == newboard.getFen());
        }

        SUBCASE("encode and decode with white castling bug") {
            auto compressed = Board::Compact::encode("rnb1kbnR/pppp4/5q2/4pp2/8/8/PPPPPP1P/RNBQKBNR b KQq - 0 1");
            auto newboard   = Board::Compact::decode(compressed);

            CHECK("rnb1kbnR/pppp4/5q2/4pp2/8/8/PPPPPP1P/RNBQKBNR b KQq - 0 1" == newboard.getFen());
        }

        SUBCASE("encode and decode with white castling queen") {
            auto compressed = Board::Compact::encode("4k1n1/pppppppp/8/8/8/8/PPPPPPPP/R3K3 w Q - 0 1");
            auto newboard   = Board::Compact::decode(compressed);

            CHECK("4k1n1/pppppppp/8/8/8/8/PPPPPPPP/R3K3 w Q - 0 1" == newboard.getFen());
        }

        SUBCASE("encode and decode with white castling king") {
            auto compressed = Board::Compact::encode("4k1n1/pppppppp/8/8/8/8/PPPPPPPP/4K2R w K - 0 1");
            auto newboard   = Board::Compact::decode(compressed);

            CHECK("4k1n1/pppppppp/8/8/8/8/PPPPPPPP/4K2R w K - 0 1" == newboard.getFen());
        }

        SUBCASE("encode and decode with black castling queen") {
            auto compressed = Board::Compact::encode("r3k1n1/pppppppp/8/8/8/8/PPPPPPPP/4K3 w q - 0 1");
            auto newboard   = Board::Compact::decode(compressed);

            CHECK("r3k1n1/pppppppp/8/8/8/8/PPPPPPPP/4K3 w q - 0 1" == newboard.getFen());
        }

        SUBCASE("encode and decode with black castling king") {
            auto compressed = Board::Compact::encode("4k1nr/pppppppp/8/8/8/8/PPPPPPPP/4K3 w k - 0 1");
            auto newboard   = Board::Compact::decode(compressed);

            CHECK("4k1nr/pppppppp/8/8/8/8/PPPPPPPP/4K3 w k - 0 1" == newboard.getFen());
        }

        SUBCASE("encode and decode with black side to move") {
            auto compressed = Board::Compact::encode("4k1n1/pppppppp/8/8/8/8/PPPPPPPP/4K3 b - - 0 1");
            auto newboard   = Board::Compact::decode(compressed);

            CHECK("4k1n1/pppppppp/8/8/8/8/PPPPPPPP/4K3 b - - 0 1" == newboard.getFen());
        }
    }

    TEST_CASE("PackedBoard from Chess960 FEN") {
        SUBCASE("castling") {
            auto compressed = Board::Compact::encode("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w HAha - 0 1", true);
            auto newboard   = Board::Compact::decode(compressed, true);

            CHECK("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w HAha - 0 1" == newboard.getFen());
        }

        SUBCASE("castling 2") {
            auto compressed =
                Board::Compact::encode("1rqbkrbn/1ppppp1p/1n6/p1N3p1/8/2P4P/PP1PPPP1/1RQBKRBN w FBfb - 0 1", true);
            auto newboard = Board::Compact::decode(compressed, true);

            CHECK("1rqbkrbn/1ppppp1p/1n6/p1N3p1/8/2P4P/PP1PPPP1/1RQBKRBN w FBfb - 0 1" == newboard.getFen());
        }

        SUBCASE("castling 3") {
            auto compressed =
                Board::Compact::encode("rbbqn1kr/pp2p1pp/6n1/2pp1p2/2P4P/P7/BP1PPPP1/R1BQNNKR w HAha - 0 1", true);
            auto newboard = Board::Compact::decode(compressed, true);

            CHECK("rbbqn1kr/pp2p1pp/6n1/2pp1p2/2P4P/P7/BP1PPPP1/R1BQNNKR w HAha - 0 1" == newboard.getFen());
        }

        SUBCASE("castling 4") {
            auto compressed =
                Board::Compact::encode("rqbbknr1/1ppp2pp/p5n1/4pp2/P7/1PP5/1Q1PPPPP/R1BBKNRN w GAga - 0 1", true);
            auto newboard = Board::Compact::decode(compressed, true);

            CHECK("rqbbknr1/1ppp2pp/p5n1/4pp2/P7/1PP5/1Q1PPPPP/R1BBKNRN w GAga - 0 1" == newboard.getFen());
        }

        SUBCASE("castling 5") {
            auto compressed =
                Board::Compact::encode("4rrb1/1kp3b1/1p1p4/pP1Pn2p/5p2/1PR2P2/2P1NB1P/2KR1B2 w D - 0 1", true);
            auto newboard = Board::Compact::decode(compressed, true);

            CHECK("4rrb1/1kp3b1/1p1p4/pP1Pn2p/5p2/1PR2P2/2P1NB1P/2KR1B2 w D - 0 1" == newboard.getFen());
        }

        SUBCASE("castling 6") {
            auto compressed =
                Board::Compact::encode("1rkr3b/1ppn3p/3pB1n1/6q1/R2P4/4N1P1/1P5P/2KRQ1B1 b Ddb - 0 1", true);
            auto newboard = Board::Compact::decode(compressed, true);

            CHECK("1rkr3b/1ppn3p/3pB1n1/6q1/R2P4/4N1P1/1P5P/2KRQ1B1 b Ddb - 0 1" == newboard.getFen());
        }

        SUBCASE("castling 7") {
            auto compressed =
                Board::Compact::encode("qbbnrkr1/p1pppppp/1p4n1/8/2P5/6N1/PPNPPPPP/1BRKBRQ1 b FCge - 0 1", true);
            auto newboard = Board::Compact::decode(compressed, true);

            CHECK("qbbnrkr1/p1pppppp/1p4n1/8/2P5/6N1/PPNPPPPP/1BRKBRQ1 b FCge - 0 1" == newboard.getFen());
        }

        SUBCASE("castling 8") {
            auto compressed =
                Board::Compact::encode("rr6/2kpp3/1ppn2p1/p2b1q1p/P4P1P/1PNN2P1/2PP4/1K2R2R b E - 0 1", true);
            auto newboard = Board::Compact::decode(compressed, true);

            CHECK("rr6/2kpp3/1ppn2p1/p2b1q1p/P4P1P/1PNN2P1/2PP4/1K2R2R b E - 0 1" == newboard.getFen());
        }

        SUBCASE("castling 9") {
            auto compressed =
                Board::Compact::encode("rr6/2kpp3/1ppnb1p1/p2Q1q1p/P4P1P/1PNN2P1/2PP4/1K2RR2 b E - 0 1", true);
            auto newboard = Board::Compact::decode(compressed, true);

            CHECK("rr6/2kpp3/1ppnb1p1/p2Q1q1p/P4P1P/1PNN2P1/2PP4/1K2RR2 b E - 0 1" == newboard.getFen());
        }
    }
}
