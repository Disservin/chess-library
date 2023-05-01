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

int main() {
#ifdef PGO_DEPTH
    constexpr int kDepth = PGO_DEPTH;
    Board board = Board();
#else
    constexpr int kDepth = 7;
    Board board = Board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
#endif  // DEPTH

    const auto t0 = std::chrono::high_resolution_clock::now();
    uint64_t nodes = perft_thread<kDepth>(board);
    for (auto& future : futures) {
        nodes += future.get();
    }
    const auto t1 = std::chrono::high_resolution_clock::now();

    const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
    std::cout << "nodes " << nodes << " time " << elapsed << " nps "
              << (nodes * 1000) / (elapsed + 1) << std::endl;

    return 0;
}

// template instantiation
template uint64_t perft_thread<1>(Board&);
template uint64_t perft_thread<2>(Board&);
template uint64_t perft_thread<3>(Board&);
template uint64_t perft_thread<4>(Board&);
template uint64_t perft_thread<5>(Board&);
template uint64_t perft_thread<6>(Board&);
template uint64_t perft_thread<7>(Board&);
template uint64_t perft_thread<8>(Board&);
template uint64_t perft_thread<9>(Board&);
template uint64_t perft_thread<10>(Board&);
template uint64_t perft_thread<11>(Board&);
template uint64_t perft_thread<12>(Board&);
template uint64_t perft_thread<13>(Board&);
template uint64_t perft_thread<14>(Board&);
template uint64_t perft_thread<15>(Board&);
template uint64_t perft_thread<16>(Board&);
template uint64_t perft_thread<17>(Board&);
template uint64_t perft_thread<18>(Board&);
template uint64_t perft_thread<19>(Board&);
template uint64_t perft_thread<20>(Board&);
template uint64_t perft_thread<21>(Board&);
template uint64_t perft_thread<22>(Board&);
template uint64_t perft_thread<23>(Board&);
template uint64_t perft_thread<24>(Board&);
template uint64_t perft_thread<25>(Board&);
template uint64_t perft_thread<26>(Board&);
template uint64_t perft_thread<27>(Board&);
template uint64_t perft_thread<28>(Board&);
template uint64_t perft_thread<29>(Board&);
template uint64_t perft_thread<30>(Board&);