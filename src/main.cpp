#include <iostream>

#include "chess.hpp"

using namespace chess;

int main() {
    Board board = Board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    std::cout << "board: " << board << std::endl;

    std::ifstream file("64a8ced102cd07745c612a6f-0.pgn");

    while (true) {
        auto game = pgn::readGame(file);

        if (!game.has_value()) {
            return 0;
        }

        if (game.value().moves().size() != std::stoi(game.value().headers().at("PlyCount"))) {
            std::cout << "Error: moves and result don't match" << std::endl;
        }
    }

    return 0;
}