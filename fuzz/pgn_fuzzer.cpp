#include <chrono>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

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

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    if (size == 0) return 0;

    std::string pgn_content(reinterpret_cast<const char*>(data), size);

    std::istringstream input_stream(pgn_content);

    auto vis = std::make_unique<MyVisitor>();

    pgn::StreamParser parser(input_stream);
    parser.readGames(*vis);

    return 0;
}