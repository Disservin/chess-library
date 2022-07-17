## Overview:
A chess library for C++.<br>
Reasonably fast move generation for all purposes.<br>
Perft(6) `nodes: 119060324 nps 282133469`
Stockfish has `Nodes/second    : 247526661` although its also updating its NNUE during that.

Sample program of perft

```c++
#include "chess.hpp"

using namespace Chess;

uint64_t nodes;

uint64_t perft(Board& board, int depth) {
    Moves moveList = board.legal_moves();
    if (depth == 1) {
        return (int)moveList.count;
    }
    uint64_t nodes = 0;
    for (int i = 0; i < (int)moveList.count; i++) {
        Move move = moveList.moves[i];
        board.make_move(move);
        nodes += perft(board, depth - 1);
        board.unmake_move(move);
    }
    return nodes;
}

int main() {
    Board board = Board();
    auto t1 = std::chrono::high_resolution_clock::now();
    uint64_t n = perft(board, 6);
    auto t2 = std::chrono::high_resolution_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
    
    std::cout << "nodes " << n << " nps " << (n * 1000) / ms << std::endl;
    return 0;
}
```

Functions:
```
board.parseFEN(string FEN)

board.print()

board.isCheckmate()

board.isStalemate()

board.isCheck()

board.ply()

board.fullmoves()

board.givesCheck<Color>(Move)

board.legal_moves()

board.make_move(Move)

board.unmake_move(Move)

board.piece_at(Square)

board.piece_type_at(Square)

board.Pawns<Color>()
board.Knights<Color>()
board.Bishops<Color>()
board.Rooks<Color>()
board.Queens<Color>()
board.Kings<Color>()

```

