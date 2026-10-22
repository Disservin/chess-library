# Examples

Below you find an example program which generates all legal moves  
for the current position and outputs them to the console.

```cpp
#include <iostream>
#include "chess.hpp"

using namespace chess;

int main () {
    Board board = Board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    Movelist moves;
    movegen::legalmoves(moves, board);

    for (const auto &move : moves) {
        std::cout << uci::moveToUci(move) << std::endl;
    }

    return 0;
}

```

Save this as `example.cpp` with `chess.hpp` on your include path, then compile
and run it with a C++17 compiler:

```bash
g++ -std=c++17 -O3 -march=native -o example example.cpp
./example
```

## Perft

Perft counts legal move sequences of a fixed length, without draw adjudication.
The function below assumes `chess.hpp` and `using namespace chess;` as above.
For the standard starting position, depths 0–3 give 1, 20, 400, and 8,902 nodes.


```cpp
// depth must be nonnegative; depth 0 counts the current position.
uint64_t perft(Board& board, int depth) {
    if (depth == 0) return 1;

    Movelist moves;
    movegen::legalmoves(moves, board);

    if (depth == 1) {
        return moves.size();
    }

    uint64_t nodes = 0;

    for (int i = 0; i < moves.size(); i++) {
        const auto move = moves[i];
        board.makeMove(move);
        nodes += perft(board, depth - 1);
        board.unmakeMove(move);
    }

    return nodes;
}
```

## PGN parser

The [PGN parsing guide](/pages/pgn-parsing#process-games-with-a-visitor) includes
a complete visitor example with file-open and parser error handling, plus a
single-pass game range example. It also explains which annotations are retained
and how to replay variations.
