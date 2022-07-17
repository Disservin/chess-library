#include "chess.hpp"

using namespace Chess;
using namespace Testing;

// Board board = Board();
// uint64_t nodes;

// template<Color c>
// uint64_t perft_driver(int depth) {
//     Moves moveList = board.generateLegalMoves<c>();
//     if (depth == 1) {
//         return (int)moveList.count;
//     }
//     uint64_t nodes = 0;
//     for (int i = 0; i < (int)moveList.count; i++) {
//         Move move = moveList.moves[i];
//         board.makemove<c>(move);
//         nodes += perft<~c>(depth - 1);
//         board.unmakemove<c>(move);
//     }
//     return nodes;
// }

int main() {
    // PerftTesting perft;
    PerftTesting perft;
    perft.RunPerftTest();
    return 0;
    
    // board.parseFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    // auto t1 = std::chrono::high_resolution_clock::now();
    // uint64_t n = perft<White>(6);
    // auto t2 = std::chrono::high_resolution_clock::now();
    // auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
    
    // std::cout << "nodes: " << n << " nps " << (n * 1000) / ms << std::endl;
    // // Perft

    // return 0;
}