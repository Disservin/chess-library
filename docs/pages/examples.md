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
#include <ifstream>
#include <memory>
#include <string>

#include "chess.hpp"

using namespace chess;

class MyVisitor : public pgn::Visitor {
   public:
    void header(std::string_view key, std::string_view value) {}

    void move(std::string_view move, std::string_view comment) {}

    void startPgn() {}
    void startMoves() {}
    void endPgn() {}
};

int main(int argc, char const *argv[]) {
    const auto file  = "games.pgn";
    auto file_stream = std::ifstream(file);

    auto vis = std::make_unique<MyVisitor>();

    pgn::StreamParser parser(file_stream);
    parser.readGames(*vis);

    return 0;
}
```
