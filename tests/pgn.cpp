#include <memory>

#include "../src/chess.hpp"
#include "doctest/doctest.hpp"

using namespace chess;

class MyVisitor : public pgn::Visitor {
   public:
    void header(std::string_view key, std::string_view value) {}

    void move(std::string_view move, std::string_view comment) { count_++; }

    void startPgn() { game_count_++; }
    void startMoves() { assert(end_count_ == game_count_ - 1); }
    void endPgn() {
        end_count_++;
        assert(end_count_ == game_count_);
    }

    int count() const { return count_; }
    int gameCount() const { return game_count_; }

   private:
    int end_count_  = 0;
    int game_count_ = 0;
    int count_      = 0;
};

TEST_SUITE("PGN StreamParser") {
    TEST_CASE("Basic PGN") {
        const auto file  = "./pgns/basic.pgn";
        auto file_stream = std::ifstream(file);

        auto vis = std::make_unique<MyVisitor>();
        pgn::StreamParser parser(file_stream);
        parser.readGames(*vis);

        CHECK(vis->count() == 130);
    }

    TEST_CASE("Corrupted PGN") {
        const auto file  = "./pgns/corrupted.pgn";
        auto file_stream = std::ifstream(file);

        auto vis = std::make_unique<MyVisitor>();
        pgn::StreamParser parser(file_stream);
        parser.readGames(*vis);

        CHECK(vis->count() == 124);
    }

    TEST_CASE("No Moves PGN") {
        const auto file  = "./pgns/no_moves.pgn";
        auto file_stream = std::ifstream(file);

        auto vis = std::make_unique<MyVisitor>();
        pgn::StreamParser parser(file_stream);
        parser.readGames(*vis);

        CHECK(vis->count() == 0);
    }

    TEST_CASE("Multiple") {
        const auto file  = "./pgns/multiple.pgn";
        auto file_stream = std::ifstream(file);

        auto vis = std::make_unique<MyVisitor>();
        pgn::StreamParser parser(file_stream);
        parser.readGames(*vis);

        CHECK(vis->gameCount() == 4);
    }
}