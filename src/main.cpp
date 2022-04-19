#include <iostream>
#include <chrono>

#include "chess.hpp"

using namespace Chess;

Board board = Board();

template <Color c>
unsigned long long perft(int depth) {
    
    if (depth == 0) {
        return 1;
    }

    unsigned long long nodes = 0;
    unsigned long long total = 0;
    Moves moveList = board.generateLegalMoves<c>();
    for (int i = 0; i < (int)moveList.count; i++) {
        Move move = moveList.moves[i];
        board.makemove<c>(move);
        nodes += perft<~c>(depth - 1);
        board.unmakemove<c>(move);
    }

    return nodes;
}

unsigned long long perftTest(int depth) {
    auto t1 = std::chrono::high_resolution_clock::now();
    unsigned long long nodes = perft<White>(depth);
    auto t2 = std::chrono::high_resolution_clock::now();

    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
    
    std::cout << "Depth " << depth << "\n";
    std::cout << "Nodes: " << nodes << "\n";
    std::cout << "Time: " << ms.count() << " ms\n";
    std::cout << "\n\n";
    return nodes;
}

int main() {
    int count = 0;
    auto t1 = std::chrono::high_resolution_clock::now();
    perftTest(1);
    perftTest(2);
    perftTest(3);
    perftTest(4);
    perftTest(5);
    if (perftTest(6) == 119060324){
        std::cout << "Test passed!\n";
        count++;
    }  
    std::cout << "Done\n---------------------------\n";
    board.parseFEN("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ");
    perftTest(1);
    perftTest(2);
    perftTest(3);
    perftTest(4);
    if (perftTest(5) == 193690690){
        std::cout << "Test passed!\n";
        count++;
    }
    std::cout << "Done\n---------------------------\n";
    board.parseFEN("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - ");
    perftTest(1);
    perftTest(2);
    perftTest(3);
    perftTest(4);
    perftTest(5);
    perftTest(6);
    if (perftTest(7) == 178633661){
        std::cout << "Test passed!\n";
        count++;
    }
    std::cout << "Done\n---------------------------\n";
    board.parseFEN("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");
    perftTest(1);
    perftTest(2);
    perftTest(3);
    perftTest(4);
    if (perftTest(5) == 15833292){
        std::cout << "Test passed!\n";
        count++;
    }
    std::cout << "Done\n---------------------------\n";
    board.parseFEN("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8  ");
    perftTest(1);
    perftTest(2);
    perftTest(3);
    perftTest(4);
    if (perftTest(5) == 89941194){
        std::cout << "Test passed!\n";
        count++;
    }
    std::cout << "Done\n---------------------------\n";
    board.parseFEN("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10 ");
    perftTest(1);
    perftTest(2);
    perftTest(3);
    perftTest(4);
    if (perftTest(5) == 164075551){
        std::cout << "Test passed!\n";
        count++;
    }
    auto t2 = std::chrono::high_resolution_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
    std::cout << "Time: " << ms.count() << " ms\n";
    if (count == 6){
        std::cout << "All tests passed!\n";
    }
    
    return 0;
}