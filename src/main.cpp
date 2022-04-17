#include <iostream>
#include <chrono>

#include "chess.hpp"

using namespace Chess;

Board board = Board();

unsigned long long perft(int depth) {
    if (depth == 0) {
        return 1;
    }

    unsigned long long nodes = 0;
    unsigned long long total = 0;
    Moves moveList = board.generateLegalMoves();
    for (int i = 0; i < (int)moveList.count; i++) {
        Move move = moveList.moves[i];
        board.makemove(move);
        nodes += perft(depth - 1);
        board.unmakemove();
    }

    return nodes;
}

void perftTest(int depth) {
    auto t1 = std::chrono::high_resolution_clock::now();
    int nodes = perft(depth);
    auto t2 = std::chrono::high_resolution_clock::now();

    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
    
    std::cout << "Depth " << depth << "\n";
    std::cout << "Nodes: " << nodes << "\n";
    std::cout << "Time: " << ms.count() << " ms\n";
    std::cout << "\n\n";
}

int main() {
    board.parseFEN("rnbqkbnr/2pppppp/p7/Pp6/8/8/1PPPPPPP/RNBQKBNR w KQkq b6 0 1");
    board.print();

    Move enp = Move(SQ_A5, SQ_B6, WhitePawn, None, 1, 0, 1, 0);
    board.makemove(enp);
    board.print();
}