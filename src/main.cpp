#include <iostream>

#include "chess.hpp"

using namespace Chess;

int main() {
    Board board = Board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR b KQkq e4 0 1 ");
    board.print();
}