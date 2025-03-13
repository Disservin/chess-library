#include <chrono>
#include <filesystem>
#include <fstream>

#include "../include/chess.hpp"

using namespace chess;

class MyVisitor : public pgn::Visitor {
   public:
    virtual ~MyVisitor() {}

    void startPgn() {}

    void header(std::string_view key, std::string_view value) {}

    void startMoves() {}

    void move(std::string_view move, std::string_view comment) {}

    void endPgn() {}

   private:
    Board board;
};

int main(int argc, char const* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <pgn_file>\n";
        return 1;
    }

    const auto file  = argv[1];
    auto file_stream = std::ifstream(file);

    auto vis = std::make_unique<MyVisitor>();

    const auto t0 = std::chrono::high_resolution_clock::now();

    pgn::StreamParser parser(file_stream);
    auto error = parser.readGames(*vis);

    if (error) {
        std::cerr << "Error: " << error.message() << "\n";
        return 1;
    }

    const auto t1 = std::chrono::high_resolution_clock::now();

    const auto file_size_mb = std::filesystem::file_size(file) / 1000.0 / 1000.0;

    std::cout << "MB/s: "
              << (file_size_mb / (std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count() / 1000.0))
              << "\n";

    std::cout << (std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count() / 1000.0) << "\n";

    return 0;
}
