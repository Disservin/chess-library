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

    TEST_CASE("xFEN python-chess ports") {
        SUBCASE("Wiki example") {
            const auto xfen = std::string("rn2k1r1/ppp1pp1p/3p2p1/5bn1/P7/2N2B2/1PPPPP2/2BNK1RR w Gkq - 4 11");
            auto board      = Board::fromXfen(xfen);
            const auto cr   = board.castlingRights();

            CHECK(board.chess960());
            CHECK(cr.getRookFile(Color::WHITE, Board::CastlingRights::Side::KING_SIDE) == File::FILE_G);
            CHECK(cr.getRookFile(Color::BLACK, Board::CastlingRights::Side::KING_SIDE) == File::FILE_G);
            CHECK(cr.getRookFile(Color::BLACK, Board::CastlingRights::Side::QUEEN_SIDE) == File::FILE_A);
            CHECK(!cr.has(Color::WHITE, Board::CastlingRights::Side::QUEEN_SIDE));

            CHECK(board.getFen() == "rn2k1r1/ppp1pp1p/3p2p1/5bn1/P7/2N2B2/1PPPPP2/2BNK1RR w Gga - 4 11");
            CHECK(board.getXfen() == xfen);

            CHECK(cr.has(Color::WHITE));
            CHECK(cr.has(Color::BLACK));
            CHECK(cr.has(Color::BLACK, Board::CastlingRights::Side::KING_SIDE));
            CHECK(cr.has(Color::WHITE, Board::CastlingRights::Side::KING_SIDE));
            CHECK(cr.has(Color::BLACK, Board::CastlingRights::Side::QUEEN_SIDE));
        }

        SUBCASE("Chess960 position 284") {
            const auto xfen = std::string("rkbqrbnn/pppppppp/8/8/8/8/PPPPPPPP/RKBQRBNN w KQkq - 0 1");
            auto board      = Board::fromXfen(xfen);
            const auto cr   = board.castlingRights();

            CHECK(cr.getRookFile(Color::WHITE, Board::CastlingRights::Side::KING_SIDE) == File::FILE_E);
            CHECK(cr.getRookFile(Color::WHITE, Board::CastlingRights::Side::QUEEN_SIDE) == File::FILE_A);
            CHECK(cr.getRookFile(Color::BLACK, Board::CastlingRights::Side::KING_SIDE) == File::FILE_E);
            CHECK(cr.getRookFile(Color::BLACK, Board::CastlingRights::Side::QUEEN_SIDE) == File::FILE_A);

            CHECK(board.getXfen() == xfen);
            CHECK(board.getFen() == "rkbqrbnn/pppppppp/8/8/8/8/PPPPPPPP/RKBQRBNN w EAea - 0 1");
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

}
