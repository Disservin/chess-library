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

    void startPgn() {
        moves_.clear();
        comments_.clear();
        count_ = 0;
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

    TEST_CASE("Skip Variations") {
        const auto file  = "./tests/pgns/variations.pgn";
        auto file_stream = std::ifstream(file);

        auto vis = std::make_unique<MyVisitor>();
        SmallBufferStreamParser parser(file_stream);
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
}
