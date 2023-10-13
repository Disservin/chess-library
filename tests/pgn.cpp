#include <memory>

#include "../src/chess.hpp"
#include "doctest/doctest.hpp"

using namespace chess;

class MyVisitor : public pgn::Visitor {
   public:
    ~MyVisitor() {}

    void startPgn() { game_count_++; }

    void header(std::string_view key, std::string_view value) {}

    void startMoves() {
        move_start_count_++;
        assert(end_count_ == game_count_ - 1);
    }

    void move(std::string_view move, std::string_view comment) { count_++; }

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
        const auto file  = "./pgns/basic.pgn";
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
        const auto file  = "./pgns/corrupted.pgn";
        auto file_stream = std::ifstream(file);

        auto vis = std::make_unique<MyVisitor>();
        pgn::StreamParser parser(file_stream);
        parser.readGames(*vis);

        CHECK(vis->count() == 124);
        CHECK(vis->gameCount() == 1);
        CHECK(vis->endCount() == 1);
        CHECK(vis->moveStartCount() == 1);
    }

    TEST_CASE("No Moves PGN") {
        const auto file  = "./pgns/no_moves.pgn";
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
        const auto file  = "./pgns/multiple.pgn";
        auto file_stream = std::ifstream(file);

        auto vis = std::make_unique<MyVisitor>();
        pgn::StreamParser parser(file_stream);
        parser.readGames(*vis);

        CHECK(vis->gameCount() == 4);
        CHECK(vis->endCount() == 4);
    }
}