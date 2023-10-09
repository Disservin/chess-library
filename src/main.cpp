#include <chrono>
#include <filesystem>

#include "./chess.hpp"

using namespace chess;

std::int64_t count = 0;

void dosomething(Game& game) {
    Board board;
    for (const auto& move : game.moves()) {
        board.makeMove(move.move);
        count++;
    }
}

int main(int argc, char const* argv[]) {
    const auto file  = "../../../lichess_db_standard_rated_2014-01.pgn";
    auto file_stream = std::ifstream(file);

    std::uint64_t count = 0;

    const auto t0 = std::chrono::high_resolution_clock::now();

    pgn::StreamParser parser(file_stream);
    parser.readGame(dosomething);

    const auto t1 = std::chrono::high_resolution_clock::now();

    const auto file_size_mb = std::filesystem::file_size(file) / 1024.0 / 1024.0;

    std::cout << "MB/s: "
              << (file_size_mb / std::chrono::duration_cast<std::chrono::seconds>(t1 - t0).count())
              << "\n";

    return 0;
}