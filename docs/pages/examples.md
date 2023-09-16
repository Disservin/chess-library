# Example

Below you find an example program which generates all legal moves  
for the current position and outputs them to the console.

```cpp
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

Then just compile the program using clang or gcc and run it.

```bash
$ g++ -std=c++17 -O3 -march=native -o example example.cpp
$ ./example
```

## Perft

```cpp
uint64_t perft(Board& board, int depth) {
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

## PGN Parser

```cpp
#include <iostream>

#include "chess.hpp"

using namespace chess;

int main() {
    std::ifstream file("game.pgn");

    while (true) {
        auto game = pgn::readGame(file);

        if (!game.has_value()) {
            break;
        }

        Board board = Board(game.value().headers().at("FEN"));

        for (auto &&i : game.value().headers()) {
            std::cout << i.first << ": " << i.second << std::endl;
        }

        for (auto &&[move, comment] : game.value().moves()) {
            std::cout << uci::moveToSan(board, move) << std::endl;
            board.makeMove(move);
        }
    }

    return 0;
}

```