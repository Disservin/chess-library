#include <iomanip>
#include <sstream>
#include <thread>

#include "chess.hpp"
#include "threadpool.hpp"

using namespace Chess;

ThreadPool pool(std::thread::hardware_concurrency());
std::vector<std::future<uint64_t>> futures;

template <int DEPTH>
uint64_t perft_thread(Board& board);

template <int DEPTH>
uint64_t perft_thread_start(Board board) {
    return perft_thread<DEPTH>(board);
}

template <int DEPTH>
uint64_t perft_thread(Board& board) {
    Movelist<Move> moves;
    Movegen::legalmoves<Move, MoveGenType::ALL>(moves, board);

    if constexpr (DEPTH == 1) {
        return moves.size();
    } else {
        uint64_t nodes = 0;

        for (int i = 0; i < moves.size(); i++) {
            const auto move = moves[i];
            board.makeMove(move);

            if constexpr (DEPTH == 5) {
                futures.emplace_back(pool.enqueue(perft_thread_start<DEPTH - 1>, board));
            } else {
                nodes += perft_thread<DEPTH - 1>(board);
            }

            board.unmakeMove(move);
        }

        return nodes;
    }
}

uint64_t evaluate(Board& board, int n);

int main() {
#ifdef PGO_DEPTH
    constexpr int kDepth = PGO_DEPTH;
    Board board = Board();
#else
    int kDepth = 7;
    std::cin >> kDepth;
    Board board = Board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
#endif  // DEPTH

    const auto t0 = std::chrono::high_resolution_clock::now();
    uint64_t nodes = evaluate(board, kDepth);
    for (auto& future : futures) {
        nodes += future.get();
    }
    const auto t1 = std::chrono::high_resolution_clock::now();

    const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
    std::cout << "nodes " << nodes << " time " << elapsed << "ms nps "
              << (nodes * 1000) / (elapsed + 1) << std::endl;

    return 0;
}

template <int N>
constexpr uint64_t evaluateHelper(Board& board) {
    return perft_thread<N>(board);
}

template <int N>
constexpr uint64_t evaluateSwitch(Board& board, int n) {
    if (n == N) {
        return evaluateHelper<N>(board);
    } else {
        return evaluateSwitch<N - 1>(board, n);
    }
}

template <>
constexpr uint64_t evaluateSwitch<0>(Board& board, int n) {
    return 0;
}

uint64_t evaluate(Board& board, int n) { return evaluateSwitch<30>(board, n); }