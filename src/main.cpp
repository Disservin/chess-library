#include <iostream>

#include "chess.hpp"

using namespace chess;

int main() {
    Board board = Board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    std::cout << "board: " << board << std::endl;

    return 0;
}