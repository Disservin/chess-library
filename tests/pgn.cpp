#include <algorithm>
#include <cassert>
#include <fstream>
#include <memory>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

#include "../src/include.hpp"
#include "doctest/doctest.hpp"

using namespace chess;

class MyVisitor : public pgn::Visitor {
   public:
    ~MyVisitor() {}

    void startPgn() {
        moves_.clear();
        comments_.clear();
        count_ = 0;
        variation_start_count_ = 0;
        variation_end_count_ = 0;
        game_count_++;
    }

    void header(std::string_view key, std::string_view value) {
        headers_.push_back(std::string(key) + " " + std::string(value));
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

    void startVariation() { variation_start_count_++; }
    void endVariation() { variation_end_count_++; }

    void endPgn() { end_count_++; }

    int count() const { return count_; }
    int gameCount() const { return game_count_; }
    int endCount() const { return end_count_; }
    int moveStartCount() const { return move_start_count_; }
    int variationStartCount() const { return variation_start_count_; }
    int variationEndCount() const { return variation_end_count_; }
    const auto& comments() const { return comments_; }
    const auto& moves() const { return moves_; }
    const auto& headers() const { return headers_; }

   private:
    std::vector<std::string> comments_;
    std::vector<std::string> moves_;
    std::vector<std::string> headers_;

    int end_count_        = 0;
    int game_count_       = 0;
    int count_            = 0;
    int move_start_count_ = 0;
    int variation_start_count_ = 0;
    int variation_end_count_ = 0;
};

class MyVisitor2 : public pgn::Visitor {
   public:
    ~MyVisitor2() {}

    void startPgn() {
        moves_.clear();
        comments_.clear();
        count_ = 0;
        game_count_++;
    }

    void header(std::string_view key, std::string_view value) {
        headers_.push_back(std::string(key) + " " + std::string(value));
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
    const auto& comments() const { return comments_; }
    const auto& moves() const { return moves_; }
    const auto& headers() const { return headers_; }

   private:
    std::vector<std::string> comments_;
    std::vector<std::string> moves_;
    std::vector<std::string> headers_;

    int end_count_        = 0;
    int game_count_       = 0;
    int count_            = 0;
    int move_start_count_ = 0;
};

class MockVisitorGameover : public pgn::Visitor {
   public:
    ~MockVisitorGameover() {}

    void startPgn() {}

    void header(std::string_view key, std::string_view value) {
        if (key == "FEN") {
            board_.setFen(value);
        }
    }

    void startMoves() {}

    void move(std::string_view move, std::string_view comment) {
        auto m = uci::parseSan(board_, move);

        board_.makeMove<true>(m);
    }

    void endPgn() { game_res_ = board_.isGameOver(); }

    auto gameResult() const { return game_res_; }

   private:
    Board board_;
    std::pair<GameResultReason, GameResult> game_res_;
};

using SmallBufferStreamParser = pgn::StreamParser<1>;

class VariationTraceVisitor : public pgn::Visitor {
   public:
    void startPgn() override {}
    void header(std::string_view, std::string_view) override {}
    void startMoves() override {}
    void move(std::string_view move, std::string_view) override {
        events.emplace_back(move);
    }
    void startVariation() override { events.emplace_back("("); }
    void endVariation() override { events.emplace_back(")"); }
    void endPgn() override {}

    std::vector<std::string> events;
};

TEST_SUITE("PGN StreamParser") {
    TEST_CASE("Basic PGN") {
        const auto file  = "./tests/pgns/basic.pgn";
        auto file_stream = std::ifstream(file);

        auto vis = std::make_unique<MyVisitor>();
        SmallBufferStreamParser parser(file_stream);
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

    TEST_CASE("Basic PGN Buffer Size 1") {
        const auto file  = "./tests/pgns/basic.pgn";
        auto file_stream = std::ifstream(file);

        auto vis = std::make_unique<MyVisitor>();
        SmallBufferStreamParser parser(file_stream);
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

    TEST_CASE("Basic PGN Buffer Size 2") {
        const auto file  = "./tests/pgns/basic.pgn";
        auto file_stream = std::ifstream(file);

        auto vis = std::make_unique<MyVisitor>();
        pgn::StreamParser<2> parser(file_stream);
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

    TEST_CASE("Basic PGN Buffer Size 3") {
        const auto file  = "./tests/pgns/basic.pgn";
        auto file_stream = std::ifstream(file);

        auto vis = std::make_unique<MyVisitor>();
        pgn::StreamParser<3> parser(file_stream);
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

    TEST_CASE("Basic PGN Buffer Size 4") {
        const auto file  = "./tests/pgns/basic.pgn";
        auto file_stream = std::ifstream(file);

        auto vis = std::make_unique<MyVisitor>();
        pgn::StreamParser<4> parser(file_stream);
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

    TEST_CASE("Basic PGN Buffer Size 5") {
        const auto file  = "./tests/pgns/basic.pgn";
        auto file_stream = std::ifstream(file);

        auto vis = std::make_unique<MyVisitor>();
        pgn::StreamParser<5> parser(file_stream);
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
        SmallBufferStreamParser parser(file_stream);
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
        SmallBufferStreamParser parser(file_stream);
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
        SmallBufferStreamParser parser(file_stream);
        parser.readGames(*vis);

        CHECK(vis->gameCount() == 4);
        CHECK(vis->endCount() == 4);
    }

    TEST_CASE("Skip") {
        const auto file  = "./tests/pgns/skip.pgn";
        auto file_stream = std::ifstream(file);

        auto vis = std::make_unique<MyVisitor>();
        SmallBufferStreamParser parser(file_stream);
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
        SmallBufferStreamParser parser(file_stream);
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
        SmallBufferStreamParser parser(file_stream);
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
        SmallBufferStreamParser parser(file_stream);
        parser.readGames(*vis);

        CHECK(vis->gameCount() == 1);
        CHECK(vis->endCount() == 1);
        CHECK(vis->moveStartCount() == 1);
        CHECK(vis->count() == 3);
    }

    TEST_CASE("Skip Variations by default") {
        const auto file  = "./tests/pgns/variations.pgn";
        auto file_stream = std::ifstream(file);

        auto vis = std::make_unique<MyVisitor>();
        SmallBufferStreamParser parser(file_stream);
        parser.readGames(*vis);

        CHECK(vis->gameCount() == 1);
        CHECK(vis->endCount() == 1);
        CHECK(vis->moveStartCount() == 1);
        CHECK(vis->count() == 108);
        CHECK(vis->variationStartCount() == 0);
        CHECK(vis->variationEndCount() == 0);
    }

    TEST_CASE("Read Variations") {
        const auto file  = "./tests/pgns/variations.pgn";
        auto file_stream = std::ifstream(file);

        auto vis = std::make_unique<MyVisitor>();
        SmallBufferStreamParser parser(file_stream);
        parser.setParseVariations(true);
        parser.readGames(*vis);

        CHECK(vis->gameCount() == 1);
        CHECK(vis->endCount() == 1);
        CHECK(vis->moveStartCount() == 1);
        CHECK(vis->count() == 212);
        CHECK(vis->variationStartCount() == 26);
        CHECK(vis->variationEndCount() == 26);
    }

    TEST_CASE("Read nested variations and preserve move order") {
        std::istringstream stream("[Event \"Nested variations\"]\n"
                                  "1. e4 e5 2. Nf3 (2. Bc4 (2... Nf6) Nc6) Nc6 *");
        VariationTraceVisitor vis;
        SmallBufferStreamParser parser(stream);
        parser.setParseVariations(true);
        const std::vector<std::string> expected = {"e4", "e5", "Nf3", "(", "Bc4", "(",
                                                   "Nf6", ")", "Nc6", ")", "Nc6"};

        CHECK(parser.readGames(vis) == pgn::StreamParserError::None);
        CHECK(vis.events == expected);
    }

    TEST_CASE("Read Book") {
        const auto file  = "./tests/pgns/book.pgn";
        auto file_stream = std::ifstream(file);

        auto vis = std::make_unique<MyVisitor>();
        SmallBufferStreamParser parser(file_stream);
        parser.readGames(*vis);

        CHECK(vis->gameCount() == 1);
        CHECK(vis->endCount() == 1);
        CHECK(vis->moveStartCount() == 1);
        CHECK(vis->count() == 16);
    }

    TEST_CASE("No Moves With Game Termination Marker 0-1") {
        const auto file  = "./tests/pgns/no_moves_but_game_termination.pgn";
        auto file_stream = std::ifstream(file);

        auto vis = std::make_unique<MyVisitor>();
        SmallBufferStreamParser parser(file_stream);
        parser.readGames(*vis);

        CHECK(vis->gameCount() == 1);
        CHECK(vis->endCount() == 1);
        CHECK(vis->moveStartCount() == 1);
        CHECK(vis->count() == 0);
    }

    TEST_CASE("No Moves With Game Termination Marker 1/2-1/2") {
        const auto file  = "./tests/pgns/no_moves_but_game_termination_2.pgn";
        auto file_stream = std::ifstream(file);

        auto vis = std::make_unique<MyVisitor>();
        SmallBufferStreamParser parser(file_stream);
        parser.readGames(*vis);

        CHECK(vis->gameCount() == 1);
        CHECK(vis->endCount() == 1);
        CHECK(vis->moveStartCount() == 1);
        CHECK(vis->count() == 0);
    }

    TEST_CASE("No Moves With Game Termination Marker *") {
        const auto file  = "./tests/pgns/no_moves_but_game_termination_3.pgn";
        auto file_stream = std::ifstream(file);

        auto vis = std::make_unique<MyVisitor>();
        SmallBufferStreamParser parser(file_stream);
        parser.readGames(*vis);

        CHECK(vis->gameCount() == 1);
        CHECK(vis->endCount() == 1);
        CHECK(vis->moveStartCount() == 1);
        CHECK(vis->count() == 0);
    }

    TEST_CASE("No Moves With Game Termination Marker Multiple") {
        const auto file  = "./tests/pgns/no_moves_but_game_termination_multiple.pgn";
        auto file_stream = std::ifstream(file);

        auto vis = std::make_unique<MyVisitor>();
        SmallBufferStreamParser parser(file_stream);
        parser.readGames(*vis);

        CHECK(vis->gameCount() == 2);
        CHECK(vis->endCount() == 2);
        CHECK(vis->moveStartCount() == 2);

        CHECK(vis->count() == 126);
        CHECK(vis->moves()[0] == "d4");
        CHECK(vis->moves()[1] == "e6");
        CHECK(vis->moves()[vis->moves().size() - 1] == "Ke4");
        CHECK(vis->moves()[vis->moves().size() - 2] == "Rd2+");
    }

    TEST_CASE("No Moves With Game Termination Marker Multiple 2") {
        const auto file  = "./tests/pgns/no_moves_but_game_termination_multiple_2.pgn";
        auto file_stream = std::ifstream(file);

        auto vis = std::make_unique<MyVisitor>();
        SmallBufferStreamParser parser(file_stream);
        parser.readGames(*vis);

        CHECK(vis->gameCount() == 3);
        CHECK(vis->endCount() == 3);
        CHECK(vis->moveStartCount() == 3);
        CHECK(vis->headers().size() == 6);

        CHECK(vis->headers()[0] == "FEN 5k2/3r1p2/1p3pp1/p2n3p/P6P/1PPR1PP1/3KN3/6b1 w - - 0 34");
        CHECK(vis->headers()[1] == "Result 1/2-1/2");

        CHECK(vis->headers()[2] == "FEN 5k2/5p2/4B2p/r5pn/4P3/5PPP/2NR2K1/8 b - - 0 59");
        CHECK(vis->headers()[3] == "Result 1/2-1/2");

        CHECK(vis->headers()[4] == "FEN 8/p3kp1p/1p4p1/2r2b2/2BR3P/1P3P2/P4PK1/8 b - - 0 28");
        CHECK(vis->headers()[5] == "Result 1/2-1/2");
    }

    TEST_CASE("No Moves But Comment Followed by Termination Marker") {
        const auto file  = "./tests/pgns/no_moves_but_comment_followed_by_termination_marker.pgn";
        auto file_stream = std::ifstream(file);

        auto vis = std::make_unique<MyVisitor>();
        SmallBufferStreamParser parser(file_stream);
        parser.readGames(*vis);

        CHECK(vis->gameCount() == 2);
        CHECK(vis->endCount() == 2);
        CHECK(vis->moveStartCount() == 2);
        CHECK(vis->count() == 1);
    }

    TEST_CASE("Threefold Repetition") {
        const auto file  = "./tests/pgns/threefold_repetition.pgn";
        auto file_stream = std::ifstream(file);

        auto vis = std::make_unique<MockVisitorGameover>();
        SmallBufferStreamParser parser(file_stream);
        parser.readGames(*vis);

        auto res = vis->gameResult();

        CHECK(res.first == GameResultReason::THREEFOLD_REPETITION);
        CHECK(res.second == GameResult::DRAW);
    }

    TEST_CASE("No Result") {
        const auto file  = "./tests/pgns/no_result.pgn";
        auto file_stream = std::ifstream(file);

        auto vis = std::make_unique<MyVisitor2>();
        SmallBufferStreamParser parser(file_stream);
        parser.readGames(*vis);

        const auto& headers = vis->headers();

        CHECK(headers[7] == "FEN r1bqk2r/pp1p1pbp/2n2np1/4p3/4P3/2NP2P1/PP2NPBP/R1BQ1RK1 w kq - 0 9");

        CHECK(headers.size() == 15);
        CHECK(vis->moves()[0] == "");
        CHECK(vis->comments()[0] == "No result");
    }

    // This is rather a bug in the pgn itself?
    TEST_CASE("Should parse game even if newline is missing after last header") {
        const auto file  = "./tests/pgns/no_moves_two_games.pgn";
        auto file_stream = std::ifstream(file);

        auto vis = std::make_unique<MyVisitor2>();
        SmallBufferStreamParser parser(file_stream);
        parser.readGames(*vis);

        CHECK(vis->endCount() == 2);
        CHECK(vis->gameCount() == 2);
        CHECK(vis->moveStartCount() == 2);
        CHECK(vis->count() == 0);
    }

    TEST_CASE("Leading whitespace before header") {
        const auto file  = "./tests/pgns/leading_whitespace.pgn";
        auto file_stream = std::ifstream(file);

        auto vis = std::make_unique<MyVisitor2>();
        SmallBufferStreamParser parser(file_stream);
        const auto error = parser.readGames(*vis);

        CHECK(error == pgn::StreamParserError::None);
        CHECK(vis->headers()[0] == "Event Batch 10: s20red4c4_t3 vs master");
        CHECK(vis->headers()[4] == "White New-cfe8ce842c");
        CHECK(vis->moves()[0] == "Bg2");
    }

    TEST_CASE("Keep track of opening square bracket") {
        const auto file  = "./tests/pgns/square_bracket_in_header.pgn";
        auto file_stream = std::ifstream(file);

        auto vis = std::make_unique<MyVisitor2>();
        SmallBufferStreamParser parser(file_stream);
        parser.readGames(*vis);

        CHECK(vis->headers()[0] == "Event Batch 10: s20red4c4_t3 vs master[!important]");
        CHECK(vis->headers()[1] == "Variation closing ] opening");
        CHECK(vis->headers()[5] == "White New-cfe8\"dsadsa\"ce842c");
    }

    TEST_CASE("Empty Body") {
        const auto file  = "./tests/pgns/empty_body.pgn";
        auto file_stream = std::ifstream(file);

        auto vis = std::make_unique<MyVisitor>();
        SmallBufferStreamParser parser(file_stream);
        parser.readGames(*vis);

        CHECK(vis->gameCount() == 2);

        CHECK(vis->headers()[0] == "Event Batch 2690: probTTsv1 vs master");
        CHECK(vis->headers()[7] == "Event Batch 269: probTTsv1 vs master");
    }

    TEST_CASE("Backslash In Header") {
        const auto file  = "./tests/pgns/backslash_header.pgn";
        auto file_stream = std::ifstream(file);

        auto vis = std::make_unique<MyVisitor>();
        SmallBufferStreamParser parser(file_stream);

        CHECK(parser.readGames(*vis) == pgn::StreamParserError::InvalidHeaderMissingClosingQuote);
        CHECK(vis->gameCount() == 1);
    }

    TEST_CASE("Comment before moves") {
        const auto file  = "./tests/pgns/comment_before_moves.pgn";
        auto file_stream = std::ifstream(file);

        auto vis = std::make_unique<MyVisitor>();
        SmallBufferStreamParser parser(file_stream);
        parser.readGames(*vis);

        // this test checks if the parser avoids an endless loop
    }

    TEST_CASE("Non Ascii") {
        const auto file  = "./tests/pgns/non_ascii.pgn";
        auto file_stream = std::ifstream(file);

        auto vis = std::make_unique<MyVisitor>();
        SmallBufferStreamParser parser(file_stream);
        parser.readGames(*vis);

        CHECK(vis->gameCount() == 1);
        CHECK(vis->endCount() == 1);
        CHECK(vis->moveStartCount() == 1);
        CHECK(vis->headers().size() == 7);

        CHECK(vis->headers()[0] == "Event Tournoi François");
        CHECK(vis->headers()[1] == "Site Café");
        CHECK(vis->headers()[2] == "Date 2024.03.15");
        CHECK(vis->headers()[3] == "Round 1");
        CHECK(vis->headers()[4] == "White José María");
        CHECK(vis->headers()[5] == "Black Владимир Петров");
        CHECK(vis->headers()[6] == "Result 1-0");
    }

    TEST_CASE("Unescaped header") {
        const auto file  = "./tests/pgns/unescaped_quote_header.pgn";
        auto file_stream = std::ifstream(file);

        auto vis = std::make_unique<MyVisitor>();
        SmallBufferStreamParser parser(file_stream);
        auto err = parser.readGames(*vis);

        CHECK(err == pgn::StreamParserError::InvalidHeaderMissingClosingBracket);

        CHECK(vis->gameCount() == 1);
        CHECK(vis->endCount() == 0);
        CHECK(vis->moveStartCount() == 0);
        CHECK(vis->headers().size() == 0);
    }
}

TEST_CASE("Buffered comments preserve bytes across refills") {
    for (std::size_t padding = 0; padding < 20; ++padding) {
        const std::string text = std::string(padding, 'x') + "\r\n" +
                                 std::string(300, 'y') + "\r\nz";
        std::string expected = text;
        expected.erase(std::remove(expected.begin(), expected.end(), '\r'), expected.end());
        for (bool closed : {false, true}) {
            std::istringstream input("[Event \"comments\"]\n\n1. e4 {" + text +
                                     (closed ? "} e5 *" : ""));
            pgn::GameRange<4> range(input);
            auto it = range.begin();
            REQUIRE(it != range.end());
            REQUIRE(!it->events.empty());
            CHECK(it->events.front().san == "e4");
            CHECK(it->events.front().comment == expected);
            CHECK(range.error() == pgn::StreamParserError::None);
        }
    }
}

TEST_CASE("Buffered move tokens preserve boundaries and length limits") {
    for (std::size_t padding = 0; padding < 17; ++padding) {
        const std::string prefix = "[Event \"tokens\"]\n\n" + std::string(padding, ' ');
        // Carriage returns inside tokens are ignored; newlines end them.
        std::istringstream input(prefix + "1. Ng\r1 e5 *");
        pgn::GameRange<4> range(input);
        auto it = range.begin();
        REQUIRE(it != range.end());
        REQUIRE(it->events.size() == 2);
        CHECK(it->events[0].san == "Ng1");
        CHECK(it->events[1].san == "e5");
        for (std::size_t length : {255, 256}) {
            std::istringstream long_input(prefix + "1. " + std::string(length, 'N') + " *");
            pgn::GameRange<4> long_range(long_input);
            auto game = long_range.begin();
            if (length == 255) {
                REQUIRE(game != long_range.end());
                REQUIRE(!game->events.empty());
                CHECK(game->events.front().san == std::string(length, 'N'));
            } else {
                CHECK(game == long_range.end());
                CHECK(long_range.error() == pgn::StreamParserError::ExceededMaxStringLength);
            }
        }
    }
}

TEST_CASE("Comment delimiters and bytes survive buffered scanning") {
    for (std::size_t padding = 0; padding < 32; ++padding) {
        for (std::size_t length : {0, 1, 15, 16, 17, 31, 32, 33, 255, 1024}) {
            const std::string body = std::string(length, 'x') + std::string("\0\x80\xff", 3) + "\r\nz";
            const std::string expected = std::string(length, 'x') + std::string("\0\x80\xff", 3) + "\nz";
            const std::string text = "[Event \"scan\"]\n\n" + std::string(padding, ' ') +
                                     "1. e4 {" + body + "} {} {tail} e5 {black} *";
            std::istringstream input(text);
            pgn::GameRange<4> range(input);
            auto game = range.begin();
            REQUIRE(game != range.end());
            REQUIRE(game->events.size() == 2);
            CHECK(game->events[0].san == "e4");
            CHECK(game->events[0].comment == expected + "tail");
            CHECK(game->events[1].san == "e5");
            CHECK(game->events[1].comment == "black");
        }
    }
}

TEST_CASE("Pending comments survive skipped variations and refills") {
    for (std::size_t padding = 0; padding < 32; ++padding) {
        std::istringstream input("[Event \"pending\"]\n\n" + std::string(padding, ' ') +
                                 "1. e4 {left} $1 (1. d4 {ignored variation comment}) {right} e5 {black} *");
        pgn::GameRange<4> range(input);
        auto game = range.begin();
        REQUIRE(game != range.end());
        REQUIRE(game->events.size() == 2);
        CHECK(game->events[0].san == "e4");
        CHECK(game->events[0].comment == "leftright");
        CHECK(game->events[1].comment == "black");
    }
}

TEST_CASE("Buffered comment views retain their trailing null byte") {
    struct CommentVisitor : pgn::Visitor {
        std::vector<std::string> comments;
        void startPgn() override {}
        void header(std::string_view, std::string_view) override {}
        void startMoves() override {}
        void move(std::string_view, std::string_view comment) override {
            CHECK(comment.data()[comment.size()] == '\0');
            comments.emplace_back(comment);
        }
        void endPgn() override {}
    };
    for (std::size_t padding = 0; padding < 32; ++padding) {
        for (const std::string body : {std::string("text"), std::string("text\r"), std::string("te\rxt"),
                                       std::string(40, 'x')}) {
            std::istringstream input("[Event \"nul\"]\n\n" + std::string(padding, ' ') +
                                     "1. e4 {" + body + "} e5 {} *");
            CommentVisitor visitor;
            pgn::StreamParser<4> parser(input);
            CHECK(parser.readGames(visitor) == pgn::StreamParserError::None);
            REQUIRE(visitor.comments.size() == 2);
            std::string expected = body;
            expected.erase(std::remove(expected.begin(), expected.end(), '\r'), expected.end());
            CHECK(visitor.comments[0] == expected);
            CHECK(visitor.comments[1].empty());
        }
    }
}

TEST_CASE("Buffered headers preserve escapes and string limits") {
    for (std::size_t padding = 0; padding < 17; ++padding) {
        const std::string prefix(padding, ' ');
        {
            std::istringstream input(prefix + "[Ev\rent \"a\\\"b\\\\c\r\nz\"]\n\n1. e4 *");
            pgn::GameRange<4> range(input);
            auto game = range.begin();
            // A newline inside a header value remains an error.
            CHECK(game == range.end());
            CHECK(range.error() == pgn::StreamParserError::InvalidHeaderMissingClosingQuote);
        }
        {
            std::istringstream input(prefix + "[Ev\rent \"a\\\"b\\\\c\rz\"]\n\n1. e4 *");
            pgn::GameRange<4> range(input);
            auto game = range.begin();
            REQUIRE(game != range.end());
            REQUIRE(game->headers.size() == 1);
            CHECK(game->headers[0].first == "Event");
            CHECK(game->headers[0].second == "a\"bcz");
        }
        for (std::size_t length : {1, 15, 16, 17, 254, 255, 256}) {
            for (bool long_key : {false, true}) {
                const std::string key = long_key ? std::string(length, 'K') : "Event";
                const std::string value = long_key ? "value" : std::string(length, 'v');
                std::istringstream input(prefix + "[" + key + " \"" + value + "\"]\n\n1. e4 *");
                pgn::GameRange<4> range(input);
                auto game = range.begin();
                if (length <= 255) {
                    REQUIRE(game != range.end());
                    REQUIRE(game->headers.size() == 1);
                    CHECK(game->headers[0].first == key);
                    CHECK(game->headers[0].second == value);
                } else {
                    CHECK(game == range.end());
                    CHECK(range.error() == pgn::StreamParserError::ExceededMaxStringLength);
                }
            }
        }
    }
}

TEST_CASE("Spaces before comments preserve whitespace and refill handling") {
    for (std::size_t padding = 0; padding < 17; ++padding) {
        for (const std::string space : {" ", "  ", " \t ", " \n ", " \r\n "}) {
            std::istringstream input("[Event \"space\"]\n\n" + std::string(padding, ' ') +
                                     "1. e4" + space + "{white}" + space + "e5" + space + "{black}" + space + "*");
            pgn::GameRange<4> range(input);
            auto game = range.begin();
            REQUIRE(game != range.end());
            REQUIRE(game->events.size() == 2);
            CHECK(game->events[0].san == "e4");
            CHECK(game->events[0].comment == "white");
            CHECK(game->events[1].san == "e5");
            CHECK(game->events[1].comment == "black");
        }
    }
}
