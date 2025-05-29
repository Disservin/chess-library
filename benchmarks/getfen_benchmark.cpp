#include <chrono>
#include <filesystem>
#include <fstream>

#include "../src/include.hpp"

using namespace chess;

int main(int argc, char const* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <fen_file>\n";
        return 1;
    }

    const auto file = argv[1];

    auto file_stream = std::ifstream(file);
    if (!file_stream.is_open()) {
        std::cerr << "Error: Could not open file " << file << "\n";
        return 1;
    }

    uint64_t count = 0;

    const auto t0 = std::chrono::high_resolution_clock::now();

    while (!file_stream.eof()) {
        std::string fen;
        std::getline(file_stream, fen);
        if (fen.empty()) continue;

        auto board    = Board::fromFen(fen);
        auto fen_back = board.getFen();
        count++;
    }

    const auto t1 = std::chrono::high_resolution_clock::now();

    file_stream.close();

    const auto file_size_mb = std::filesystem::file_size(file) / 1000.0 / 1000.0;
    const auto duration_ms  = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
    std::cout << "Processed " << count << " FEN strings.\n";
    std::cout << "MB/s: " << (file_size_mb / (duration_ms / 1000.0)) << "\n";
    std::cout << "Time taken: " << (duration_ms / 1000.0) << " seconds\n";
    std::cout << "Average time per FEN: " << (duration_ms / static_cast<double>(count)) << " ms\n";

    return 0;
}
