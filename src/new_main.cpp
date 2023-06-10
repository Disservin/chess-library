#include "chess.hpp"

using namespace chess;

U64 perft(Board &board, int depth) {
    if (depth == 0) {
        return 1;
    }

    Movelist moves;
    movegen::legalmoves(moves, board);

    if (depth == 1) {
        return moves.size();
    }

    U64 nodes = 0;
    for (int i = 0; i < moves.size(); i++) {
        board.makeMove(moves[i]);

        nodes += perft(board, depth - 1);

        board.unmakeMove(moves[i]);
    }

    return nodes;
}

int main(int argc, char const *argv[]) {
    Board board;
    std::cout << board << std::endl;

    utils::printBitboard(movegen::attacks::bishop(Square::SQ_D4, 0x80000004000420c0));

    for (size_t i = 0; i < 8; i++) {
        std::cout << perft(board, i) << std::endl;
    }

    std::cout << board.isAttacked(Square::SQ_E4, Color::WHITE) << std::endl;

    auto move = uci::parseSan(board, "e4");
    std::cout << uci::moveToUci(move, false) << std::endl;
    std::cout << uci::moveToLan(board, move) << std::endl;
    std::cout << uci::moveToSan(board, move) << std::endl;

    return 0;
}
