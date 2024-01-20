#include <cassert>
#include <fstream>
#include <memory>
#include <string_view>

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

    void move(std::string_view move, std::string_view comment) {
        count_++;

        if (comment.size() > 0) {
            comments_.push_back(std::string(comment));
        }

        moves_.push_back(std::string(move));
    }

    void endPgn() { end_count_++; }

    int count() const { return count_; }
    int gameCount() const { return game_count_; }
    int endCount() const { return end_count_; }
    int moveStartCount() const { return move_start_count_; }
    auto comments() const { return comments_; }
    auto moves() const { return moves_; }

   private:
    std::vector<std::string> comments_;
    std::vector<std::string> moves_;

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

        CHECK(vis->moves()[0] == "Bg2");
        CHECK(vis->comments()[0] == "+1.55/16 0.70s");

        CHECK(vis->moves()[1] == "O-O");
        CHECK(vis->comments()[1] == "-1.36/18 0.78s");

        CHECK(vis->moves()[2] == "O-O");
        CHECK(vis->comments()[2] == "+1.84/16 0.42s");

        CHECK(vis->moves()[3] == "a5");
        CHECK(vis->comments()[3] == "-1.30/16 0.16s");
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

        CHECK(vis->moves()[0] == "0-0-0");

        CHECK(vis->moves()[1] == "0-0");

        CHECK(vis->moves()[2] == "Nc5");
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
