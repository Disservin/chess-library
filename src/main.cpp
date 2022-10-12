#include "chess_lib.hpp"

using namespace Chess;

uint64_t nodes;

uint64_t perft(Board &board, int depth)
{
    Movelist moves;
    Movegen::legalmoves<ALL>(board, moves);

    if (depth == 1)
    {
        return (int)moves.size;
    }
    uint64_t nodes = 0;
    for (int i = 0; i < (int)moves.size; i++)
    {
        Move move = moves[i].move;
        board.makeMove(move);
        nodes += perft(board, depth - 1);
        board.unmakeMove(move);
    }
    return nodes;
}

int main()
{
    Board board = Board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    board.print();
    auto t1 = std::chrono::high_resolution_clock::now();
    uint64_t n = perft(board, 7);
    auto t2 = std::chrono::high_resolution_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();

    std::cout << "nodes " << n << " nps " << (n * 1000) / (ms + 1) << std::endl;
    return 0;
}