#include <iostream>

#include "chess.hpp"

using namespace Chess;

int main() {
    Board board = Board(trickyPosition);
    board.print();

    Moves moveList = board.generatePseudoLegalMoves();
    for (int i = 0; i < moveList.size(); i++) {
        printMove(moveList[i]);
    }
    std::cout << "Number of moves: " << moveList.size() << std::endl;
}