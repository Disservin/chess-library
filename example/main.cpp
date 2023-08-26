#include <iostream>

#include "../src/chess.hpp"

using namespace chess;

int main() {
    std::ifstream file("fishtest.pgn");

    while (true) {
        auto game = pgn::readGame(file);

        if (!game.has_value()) {
            break;
        }

        Board board = Board(game.value().headers().at("FEN"));

        for (auto &&i : game.value().headers()) {
            std::cout << i.first << ": " << i.second << std::endl;
        }

        for (auto &&[move, comment] : game.value().moves()) {
            std::cout << uci::moveToSan(board, move) << std::endl;
            board.makeMove(move);
        }
    }

    return 0;
}
