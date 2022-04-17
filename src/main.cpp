#include <iostream>
#include <chrono>

#include "chess.hpp"

using namespace Chess;

Board board = Board();

unsigned long long captures   = 0;
unsigned long long promotions = 0;
unsigned long long castles    = 0;
unsigned long long enpassants = 0;

void updateFlags(Move move) {
    if (move.capture()) captures++;
    if (move.promoted()) promotions++;
    if (move.castling()) castles++;
    if (move.enpassant()) enpassants++;
}

unsigned long long perft(int depth) {
    Moves moveList = board.generateLegalMoves();
    if (depth == 1) {
        return moveList.count;
    }

    unsigned long long nodes = 0;
    unsigned long long total = 0;
    
    for (int i = 0; i < (int)moveList.count; i++) {
        Move move = moveList.moves[i];
        board.makemove(move);
        updateFlags(move);
        nodes += perft(depth - 1);
        board.unmakemove(move);
    }

    return nodes;
}

void perftTest(int depth) {
    captures   = 0;
    promotions = 0;
    castles    = 0;
    enpassants = 0;

    auto t1 = std::chrono::high_resolution_clock::now();
    unsigned long long nodes = perft(depth);
    auto t2 = std::chrono::high_resolution_clock::now();

    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
    
    std::cout << "Depth " << depth << "\n";
    std::cout << "Nodes: " << nodes << "\n";
    std::cout << "Captures: " << captures << "\n";
    std::cout << "Promotions: " << promotions << "\n";
    std::cout << "Enpassants: " << enpassants << "\n";
    std::cout << "Castles: " << castles << "\n";
    std::cout << "Time: " << ms.count() << " ms\n";
    std::cout << "\n\n";
}

int main() {
    board.parseFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    perftTest(1);
    perftTest(2);
    perftTest(3);
    perftTest(4);
    perftTest(5);

    return 0;
}