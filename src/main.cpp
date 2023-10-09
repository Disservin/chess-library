#include <chrono>
#include <filesystem>

#include "./chess.hpp"

using namespace chess;

std::int64_t count = 0;

void dosomething(Game& game) {
    // for (const auto& header : game.headers()) {
    //     std::cout << header.first << ": " << header.second << "\n";
    // }

    Board board;
    board.setFen(game.headers().at("FEN"));

    for (const auto& move : game.moves()) {
        board.makeMove(move.move);
        count++;
    }
}

int main(int argc, char const* argv[]) {
    const auto file  = "62df67e48e4fa6ae47266770-10.pgn";
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