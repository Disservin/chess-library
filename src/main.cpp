#include "chess.hpp"
#include <iomanip>
#include <sstream>

using namespace Chess;

class PerftTest
{
  public:
    uint64_t nodes;

    uint64_t perft(Board &board, int depth)
    {
        Movelist moves;
        Movegen::legalmoves<ALL>(board, moves);

        if (depth == 1)
        {
            return int(moves.size);
        }

        uint64_t nodes = 0;

        for (int i = 0; i < int(moves.size); i++)
        {
            Move move = moves[i].move;
            board.makeMove(move);
            nodes += perft(board, depth - 1);
            board.unmakeMove(move);
        }

        // for (auto extmove : moves)
        // {
        //     Move move = extmove.move;
        //     assert(moves.find(move) != -1);
        //     board.makeMove(move);
        //     nodes += perft(board, depth - 1);
        //     board.unmakeMove(move);
        // }

        return nodes;
    }

    uint64_t testPositionPerft(Board &board, int depth, uint64_t expectedNodeCount)
    {
        nodes = 0ull;
        std::stringstream ss;

        const auto t1 = std::chrono::high_resolution_clock::now();
        const uint64_t n = perft(board, depth);
        const auto t2 = std::chrono::high_resolution_clock::now();
        const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();

        if (n != expectedNodeCount)
        {
            ss << "Wrong node count ";
        }

        ss << "depth " << std::left << std::setw(2) << depth << " time " << std::setw(5) << ms << " nodes "
           << std::setw(12) << n << " nps " << std::setw(9) << (n * 1000) / (ms + 1) << " fen " << std::setw(87)
           << board.getFen();
        std::cout << ss.str() << std::endl;

        return n;
    }
};

int main()
{
    Board board = Board(DEFAULT_POS);
    PerftTest perft = PerftTest();

    U64 totalNodes = 0;

    const auto t1 = std::chrono::high_resolution_clock::now();
    totalNodes += perft.testPositionPerft(board, 7, 3195901860);

    board.applyFen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ");
    totalNodes += perft.testPositionPerft(board, 5, 193690690);

    board.applyFen("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - ");
    totalNodes += perft.testPositionPerft(board, 7, 178633661);

    board.applyFen("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");
    totalNodes += perft.testPositionPerft(board, 6, 706045033);

    board.applyFen("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8");
    totalNodes += perft.testPositionPerft(board, 5, 89941194);

    board.applyFen("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10 ");
    totalNodes += perft.testPositionPerft(board, 5, 164075551);

    const auto t2 = std::chrono::high_resolution_clock::now();
    const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();

    std::cout << "\naveraged: \n"
              << "nodes " << totalNodes << " nps " << (totalNodes * 1000) / (ms + 1) << std::endl;

    return 0;
}