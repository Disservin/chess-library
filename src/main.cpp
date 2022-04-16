#include <iostream>

#include "chess.hpp"

using namespace Chess;

int main() {
    Board board = Board();
    board.print();

    printBitboard(GetBishopAttacks(SQ_E4, SQUARE_BB[SQ_C2]));
}