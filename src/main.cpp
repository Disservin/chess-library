#include <iostream>

#include "chess.hpp"

using namespace chess;

int main() {
    Board board = Board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    std::cout << "board: " << board << std::endl;

    std::ifstream file("../64a9d23002cd07745c613e27-0.pgn");

    while (true) {
        auto game = pgn::readGame(file);

        if (!game.has_value()) {
            return 0;
        }

        std::cout << "Reading game" << std::endl;

        for (auto move : game.value().moves()) {
            std::cout << move << "\n";
        }
    }

    return 0;
}