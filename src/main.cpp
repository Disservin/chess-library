#include <iostream>

#include "chess.hpp"

using namespace Chess;

int main() {
    Board board = Board("7q/6P1/8/2P1K2r/8/8/3p4/8 w - - 0 1");
    board.print();

    // printBitboard(GetBishopAttacks(SQ_E4, SQUARE_BB[SQ_C2]));
    // printBitboard(board.doCheckmask(White, SQ_E3));
    printBitboard(board.create_pins(White, SQ_E5));
}