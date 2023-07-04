#include <iomanip>
#include <sstream>
#include <chrono>

#include "chess.hpp"

using namespace chess;

class PerftTest {
   public:
    uint64_t nodes;
    Board board;

    uint64_t perft(int depth, int max) {
        Movelist moves;
        movegen::legalmoves(moves, board);

        if (depth == 1) {
            return moves.size();
        }

        U64 nodesIt = 0;

        for (int i = 0; i < moves.size(); i++) {
            const auto move = moves[i];
            board.makeMove(move);
            nodesIt += perft(depth - 1, depth);
            board.unmakeMove(move);
            if (depth == max) {
                nodes += nodesIt;
                nodesIt = 0;
            }
        }

        return nodesIt;
    }

    uint64_t testPositionPerft(Board &b, int depth, uint64_t expectedNodeCount) {
        nodes = 0;
        std::stringstream ss;

        board = b;

        const auto t1 = std::chrono::high_resolution_clock::now();
        perft(depth, depth);
        const auto t2 = std::chrono::high_resolution_clock::now();
        const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();

        ss << "depth " << std::left << std::setw(2) << depth << " time " << std::setw(5) << ms
           << " nodes " << std::setw(12) << nodes << " nps " << std::setw(9)
           << (nodes * 1000) / (ms + 1) << " fen " << std::setw(87) << board.getFen();

        if (nodes != expectedNodeCount) {
            ss << "Wrong node count ";
        }

        std::cout << ss.str() << std::endl;

        return nodes;
    }
};

int main() {
    Board board = Board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    PerftTest perft = PerftTest();

    U64 totalNodes = 0;

    auto t1 = std::chrono::high_resolution_clock::now();
    totalNodes += perft.testPositionPerft(board, 7, 3195901860);

    board.setFen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ");
    totalNodes += perft.testPositionPerft(board, 5, 193690690);

    board.setFen("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - ");
    totalNodes += perft.testPositionPerft(board, 7, 178633661);

    board.setFen("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");
    totalNodes += perft.testPositionPerft(board, 6, 706045033);

    board.setFen("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8");
    totalNodes += perft.testPositionPerft(board, 5, 89941194);

    board.setFen("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 1");
    totalNodes += perft.testPositionPerft(board, 5, 164075551);

    auto t2 = std::chrono::high_resolution_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();

    std::cout << "\naveraged: \n"
              << "nodes " << totalNodes << " nps " << (totalNodes * 1000) / (ms + 1) << std::endl;

    std::cout << "\nStarting Chess960 tests\n";

    board.set960(true);
    board.setFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w AHah - 0 1");

    totalNodes = 0;

    t1 = std::chrono::high_resolution_clock::now();
    totalNodes += perft.testPositionPerft(board, 6, 119060324ull);

    board.setFen("1rqbkrbn/1ppppp1p/1n6/p1N3p1/8/2P4P/PP1PPPP1/1RQBKRBN w FBfb - 0 9");
    totalNodes += perft.testPositionPerft(board, 6, 191762235ull);

    board.setFen("rbbqn1kr/pp2p1pp/6n1/2pp1p2/2P4P/P7/BP1PPPP1/R1BQNNKR w HAha - 0 9");
    totalNodes += perft.testPositionPerft(board, 6, 924181432ull);

    board.setFen("rqbbknr1/1ppp2pp/p5n1/4pp2/P7/1PP5/1Q1PPPPP/R1BBKNRN w GAga - 0 9");
    totalNodes += perft.testPositionPerft(board, 6, 308553169ull);

    board.setFen("4rrb1/1kp3b1/1p1p4/pP1Pn2p/5p2/1PR2P2/2P1NB1P/2KR1B2 w D - 0 21");
    totalNodes += perft.testPositionPerft(board, 6, 872323796ull);

    board.setFen("1rkr3b/1ppn3p/3pB1n1/6q1/R2P4/4N1P1/1P5P/2KRQ1B1 b Dbd - 0 14");
    totalNodes += perft.testPositionPerft(board, 6, 2678022813ull);

    t2 = std::chrono::high_resolution_clock::now();
    ms = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();

    std::cout << "\naveraged: \n"
              << "nodes " << totalNodes << " nps " << (totalNodes * 1000) / (ms + 1) << std::endl;

    return 0;
}