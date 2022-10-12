#include "chess.hpp"

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

using namespace std;

int main()
{
    Board board = Board(DEFAULT_POS);
    board.print();
    U64 totalNodes = 0;
    auto t1 = chrono::high_resolution_clock::now();
    uint64_t n = perft(board, 7);
    totalNodes += n;

    if (n != 3195901860)
        cout << "wrong bench 1" << endl;

    board.applyFen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ");
    n = perft(board, 5);
    totalNodes += n;

    if (n != 193690690)
        cout << "wrong bench 2" << endl;

    board.applyFen("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - ");
    n = perft(board, 7);
    totalNodes += n;

    if (n != 178633661)
        cout << "wrong bench 3" << endl;

    board.applyFen("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");
    n = perft(board, 6);
    totalNodes += n;

    if (n != 706045033)
        cout << "wrong bench 4" << endl;

    board.applyFen("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8");
    n = perft(board, 5);
    totalNodes += n;

    if (n != 89941194)
        cout << "wrong bench 5" << endl;

    board.applyFen("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10 ");
    n = perft(board, 5);
    totalNodes += n;

    if (n != 164075551)
        cout << "wrong bench 6" << endl;

    auto t2 = chrono::high_resolution_clock::now();
    auto ms = chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();

    cout << "nodes " << totalNodes << " nps " << (totalNodes * 1000) / (ms + 1) << endl;
    return 0;
}