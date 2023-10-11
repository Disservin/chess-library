#include <chrono>
#include <filesystem>

#include "./chess.hpp"

using namespace chess;

class MyVisitor : public pgn::Visitor {
   public:
    void header(const std::string &key, const std::string &value) {
        // if (key == "FEN") {
        //     board.setFen(value);
        // }
    }

    void move(const std::string &move, const std::string &comment) {
        // auto m = uci::parseSan(board, move);
        // board.makeMove(m);
    }

    void startPgn() { board.setFen(STARTPOS); }
    void startMoves() {}
    void endPgn() {}

   private:
    Board board;
};

int main(int argc, char const *argv[]) {
    const auto file  = "..\\..\\..\\lichess_db_standard_rated_2014-01.pgn";
    auto file_stream = std::ifstream(file);

    auto vis = std::make_unique<MyVisitor>();

    const auto t0 = std::chrono::high_resolution_clock::now();

    pgn::StreamParser parser(file_stream);
    parser.readGames(*vis);

    const auto t1 = std::chrono::high_resolution_clock::now();

    const auto file_size_mb = std::filesystem::file_size(file) / 1000.0 / 1000.0;

    std::cout << "MB/s: "
              << (file_size_mb / std::chrono::duration_cast<std::chrono::seconds>(t1 - t0).count())
              << "\n";

    return 0;
}