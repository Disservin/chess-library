#include <chrono>
#include <iomanip>

/*
SIMPLY CLONE THE FOLLOWING REPOSITORIES AND RUN THIS FILE.

`g++ -O3 -march=native -std=c++20 -DNEDBUG .\benchmark.cpp .\surge\src\types.cpp
.\surge\src\position.cpp .\surge\src\tables.cpp .\thc-chess-library\src\thc.cpp -o benchmark`

Surge was making problems with undefined references, so I had to manually fix it.
*/

// chess-library
#include "chess-library/include/chess.hpp"

// surge
#include "surge/src/position.h"
#include "surge/src/tables.h"
#include "surge/src/types.h"

// thc
#include "thc-chess-library/src/thc.h"

// using namespace chess;
using namespace std::chrono;

class ChessPerft {
   public:
    virtual void setup(std::string fen) = 0;

    virtual uint64_t perft(int depth) = 0;

    void benchPerft(std::string fen, int depth, uint64_t expected_node_count) {
        setup(fen);

        const auto t1    = high_resolution_clock::now();
        const auto nodes = perft(depth);
        const auto t2    = high_resolution_clock::now();
        const auto ms    = duration_cast<milliseconds>(t2 - t1).count();

        std::stringstream ss;
        // clang-format off
        ss << "depth " << std::left << std::setw(2) << depth
           << " time " << std::setw(5) << ms
           << " nodes " << std::setw(12) << nodes
           << " nps " << std::setw(9) << (nodes * 1000) / (ms + 1)
           << " fen " << std::setw(87) << fen;
        // clang-format on
        std::cout << ss.str() << std::endl;
    }
};

class DisservinChess : public ChessPerft {
   public:
    void setup(std::string fen) override { board_.setFen(fen); }

    uint64_t perft(int depth) {
        chess::Movelist moves;
        chess::movegen::legalmoves(moves, board_);

        if (depth == 1) {
            return moves.size();
        }

        chess::U64 nodes = 0;

        for (const auto& move : moves) {
            board_.makeMove(move);
            nodes += perft(depth - 1);
            board_.unmakeMove(move);
        }

        return nodes;
    }

   private:
    chess::Board board_;
};

class SurgeChess : public ChessPerft {
   public:
    SurgeChess() {
        initialise_all_databases();
        zobrist::initialise_zobrist_keys();
    }

    ~SurgeChess() { delete board_; }

    void setup(std::string fen) override {
        if (board_ != nullptr) {
            delete board_;
        }

        board_ = new Position();
        Position::set(fen, *board_);
    }

    uint64_t perft(int depth) {
        return board_->turn() == WHITE ? perft<WHITE>(depth) : perft<BLACK>(depth);
    }

    template <Color C>
    uint64_t perft(int depth) {
        MoveList<C> list(*board_);

        if (depth == 1) {
            return list.size();
        }

        chess::U64 nodes = 0;

        for (const auto& move : list) {
            board_->play<C>(move);
            nodes += perft<~C>(depth - 1);
            board_->undo<C>(move);
        }

        return nodes;
    }

   private:
    Position* board_ = nullptr;
};

class THCChess : public ChessPerft {
   public:
    void setup(std::string fen) override { cr.Forsyth(fen.c_str()); }

    // fast perft implementation according to the author
    uint64_t perft(int depth) {
        thc::MOVELIST movelist;
        int n_moves, i;
        uint64_t nodes = 0;

        if (depth == 0) return 1ULL;

        cr.GenMoveList(&movelist);
        n_moves = movelist.count;
        for (i = 0; i < n_moves; i++) {
            cr.PushMove(movelist.moves[i]);
            bool okay = cr.Evaluate();
            if (okay) nodes += perft(depth - 1);
            cr.PopMove(movelist.moves[i]);
        }
        return nodes;
    }

   private:
    thc::ChessRules cr;
};

struct Test {
    std::string fen;
    uint64_t expected_node_count;
    int depth;
};

int main() {
    const Test test_positions[] = {
        {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 119060324, 6},
        {"r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1", 193690690, 5},
        {"8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1", 11030083, 6},
        {"r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1", 15833292, 5},
        {"rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8", 89941194, 5},
        {"r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 1"
         "1",
         164075551, 5}};

    DisservinChess perft;

    std::cout << "chess-library:" << std::endl;

    for (const auto& test : test_positions) {
        perft.benchPerft(test.fen, test.depth, test.expected_node_count);
    }

    std::cout << "\nSurge:" << std::endl;

    SurgeChess surge_perft;
    for (const auto& test : test_positions) {
        surge_perft.benchPerft(test.fen, test.depth, test.expected_node_count);
    }

    std::cout << "\nTHC:" << std::endl;

    THCChess thc_perft;
    for (const auto& test : test_positions) {
        thc_perft.benchPerft(test.fen, test.depth, test.expected_node_count);
    }
}

/*
chess-library:
depth 6  time 539   nodes 119060324    nps 220482081 fen rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR
w KQkq - 0 1 depth 5  time 538   nodes 193690690    nps 359351929 fen
r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1 depth 6  time 64    nodes
11030083     nps 169693584 fen 8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1 depth 5  time 53    nodes
15833292     nps 293209111 fen r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1
depth 5  time 267   nodes 89941194     nps 335601470 fen
rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8 depth 5  time 468   nodes 164075551    nps
349841260 fen r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 11

Surge:
depth 6  time 713   nodes 119060324    nps 166751154 fen rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR
w KQkq - 0 1 depth 5  time 841   nodes 193690690    nps 230036448 fen
r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1 depth 6  time 75    nodes
11030083     nps 145132671 fen 8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1 depth 5  time 85    nodes
15833292     nps 184108046 fen r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1
depth 5  time 419   nodes 89941194     nps 214145700 fen
rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8 depth 5  time 770   nodes 164075551    nps
212808756 fen r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 11

THC:
depth 6  time 3294  nodes 119060324    nps 36133633  fen rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR
w KQkq - 0 1 depth 5  time 5043  nodes 193690690    nps 38400216  fen
r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1 depth 6  time 404   nodes
11030083     nps 27234772  fen 8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1 depth 5  time 388   nodes
15833292     nps 40702550  fen r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1
depth 5  time 2909  nodes 89941194     nps 30907626  fen
rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8 depth 5  time 3403  nodes 164075551    nps
48200808  fen r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 11
*/