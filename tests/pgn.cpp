#include <memory>
#include <string_view>
#include <cassert>
#include <fstream>

#include "../src/include.hpp"
#include "doctest/doctest.hpp"

using namespace chess;

class MyVisitor : public pgn::Visitor {
   public:
    ~MyVisitor() {}

    void startPgn() { game_count_++; }

    void header(std::string_view key, std::string_view value) {
        if (key == "Result" && value == "*") {
            this->skipPgn(true);
        }
    }

    void startMoves() {
        move_start_count_++;
        assert(end_count_ == game_count_ - 1);
    }

    void move(std::string_view, std::string_view) { count_++; }

    void endPgn() { end_count_++; }

    int count() const { return count_; }
    int gameCount() const { return game_count_; }
    int endCount() const { return end_count_; }
    int moveStartCount() const { return move_start_count_; }

   private:
    int end_count_        = 0;
    int game_count_       = 0;
    int count_            = 0;
    int move_start_count_ = 0;
};

TEST_SUITE("PGN StreamParser") {
    TEST_CASE("Basic PGN") {
        const auto file  = "./tests/pgns/basic.pgn";
        auto file_stream = std::ifstream(file);

        auto vis = std::make_unique<MyVisitor>();
        pgn::StreamParser parser(file_stream);
        parser.readGames(*vis);

        CHECK(vis->count() == 130);
        CHECK(vis->gameCount() == 1);
        CHECK(vis->endCount() == 1);
        CHECK(vis->moveStartCount() == 1);
    }

    TEST_CASE("Corrupted PGN") {
        const auto file  = "./tests/pgns/corrupted.pgn";
        auto file_stream = std::ifstream(file);

        auto vis = std::make_unique<MyVisitor>();
        pgn::StreamParser parser(file_stream);
        parser.readGames(*vis);

        CHECK(vis->count() == 125);
        CHECK(vis->gameCount() == 1);
        CHECK(vis->endCount() == 1);
        CHECK(vis->moveStartCount() == 1);
    }

    TEST_CASE("No Moves PGN") {
        const auto file  = "./tests/pgns/no_moves.pgn";
        auto file_stream = std::ifstream(file);

        auto vis = std::make_unique<MyVisitor>();
        pgn::StreamParser parser(file_stream);
        parser.readGames(*vis);

        CHECK(vis->count() == 0);
        CHECK(vis->moveStartCount() == 1);
        CHECK(vis->gameCount() == 1);
        CHECK(vis->endCount() == 1);
    }

    TEST_CASE("Multiple") {
        const auto file  = "./tests/pgns/multiple.pgn";
        auto file_stream = std::ifstream(file);

        auto vis = std::make_unique<MyVisitor>();
        pgn::StreamParser parser(file_stream);
        parser.readGames(*vis);

        CHECK(vis->gameCount() == 4);
        CHECK(vis->endCount() == 4);
    }

    TEST_CASE("Skip") {
        const auto file  = "./tests/pgns/skip.pgn";
        auto file_stream = std::ifstream(file);

        auto vis = std::make_unique<MyVisitor>();
        pgn::StreamParser parser(file_stream);
        parser.readGames(*vis);

        CHECK(vis->gameCount() == 2);
        CHECK(vis->endCount() == 2);
        CHECK(vis->moveStartCount() == 1);
        CHECK(vis->count() == 130);
    }

    TEST_CASE("Newline by moves") {
        const auto file  = "./tests/pgns/newline.pgn";
        auto file_stream = std::ifstream(file);

        auto vis = std::make_unique<MyVisitor>();
        pgn::StreamParser parser(file_stream);
        parser.readGames(*vis);

        CHECK(vis->gameCount() == 1);
        CHECK(vis->endCount() == 1);
        CHECK(vis->moveStartCount() == 1);
        CHECK(vis->count() == 6);
    }

    TEST_CASE("Castling with 0-0") {
        const auto file  = "./tests/pgns/castling.pgn";
        auto file_stream = std::ifstream(file);

        auto vis = std::make_unique<MyVisitor>();
        pgn::StreamParser parser(file_stream);
        parser.readGames(*vis);

        CHECK(vis->gameCount() == 1);
        CHECK(vis->endCount() == 1);
        CHECK(vis->moveStartCount() == 1);
        CHECK(vis->count() == 6);
    }

    TEST_CASE("Black to move, and castling with 0-0-0") {
        const auto file  = "./tests/pgns/black2move.pgn";
        auto file_stream = std::ifstream(file);

        auto vis = std::make_unique<MyVisitor>();
        pgn::StreamParser parser(file_stream);
        parser.readGames(*vis);

        CHECK(vis->gameCount() == 1);
        CHECK(vis->endCount() == 1);
        CHECK(vis->moveStartCount() == 1);
        CHECK(vis->count() == 3);
    }

    TEST_CASE("Skip Variations") {
        const auto file  = "./tests/pgns/variations.pgn";
        auto file_stream = std::ifstream(file);

        auto vis = std::make_unique<MyVisitor>();
        pgn::StreamParser parser(file_stream);
        parser.readGames(*vis);

        CHECK(vis->gameCount() == 1);
        CHECK(vis->endCount() == 1);
        CHECK(vis->moveStartCount() == 1);
        CHECK(vis->count() == 108);
    }

    TEST_CASE("Read Book") {
        const auto file  = "./tests/pgns/book.pgn";
        auto file_stream = std::ifstream(file);

        auto vis = std::make_unique<MyVisitor>();
        pgn::StreamParser parser(file_stream);
        parser.readGames(*vis);

        CHECK(vis->gameCount() == 1);
        CHECK(vis->endCount() == 1);
        CHECK(vis->moveStartCount() == 1);
        CHECK(vis->count() == 16);
    }
}
