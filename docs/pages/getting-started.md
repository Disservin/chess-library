# Getting started

chess-library is a header-only C++17 library. Copy `include/chess.hpp` into your
project's include path and include it wherever you need chess operations. No
separate library needs to be linked. All public types live in the `chess` namespace.

## Play a move and generate legal replies

```cpp
#include <iostream>
#include "chess.hpp"

int main() {
    chess::Board board; // Standard starting position.
    const auto move = chess::uci::parseSan(board, "e4");
    board.makeMove(move);

    chess::Movelist replies;
    chess::movegen::legalmoves(replies, board);
    std::cout << board.getFen() << '\n';
    for (const auto reply : replies) {
        std::cout << chess::uci::moveToUci(reply) << '\n';
    }

    board.unmakeMove(move); // Restore the position before e4.
}
```

For example, with the header beside `main.cpp`, compile using:

```sh
c++ -std=c++17 -O2 main.cpp -o example
```

Use generated legal moves or parse moves against the current board before calling
`makeMove`. The board does not serve as an input validator for arbitrary manually
constructed moves. SAN notation depends on the position; UCI notation uses source
and destination squares, such as `e2e4`.

Next, see [the Board API](/pages/board), [move formats](/pages/move#other-formats),
[PGN parsing](/pages/pgn-parsing), and [more examples](/pages/examples).

## Build configuration

The library uses assertions and may print debug information to `std::cerr`.
Defining `NDEBUG` disables assertions; keep them enabled during development to help
catch incorrect API usage. Some operations throw exceptions for invalid input.
Define `CHESS_NO_EXCEPTIONS` before including the header to disable the library's
exception paths. This does not make invalid input safe; validate external input
and check return values where the API provides them.

`CHESS_USE_PEXT` selects BMI2 PEXT-based sliding attack lookup. Define it only
when your compiler target and runtime CPU support BMI2 (for example, using
`-mbmi2` with GCC or Clang on an appropriate x86 CPU). It is optional; the default
uses magic-bitboard lookup. Use consistent configuration macros across translation
units that include the library.

## Checking game endings

Engines should avoid `isGameOver()` because it redundantly generates legal moves that your engine has already calculated.  
The following search sketch assumes your engine defines `MATED_SCORE` and
`DRAW_SCORE`. Use `isRepetition(1)` instead if your search treats an earlier single
occurrence as a draw:

```cpp
if (board.isHalfMoveDraw())
  return board.getHalfMoveDrawType().first == GameResultReason::CHECKMATE ? 
         MATED_SCORE : DRAW_SCORE;

if (board.isInsufficientMaterial() || board.isRepetition())
  return DRAW_SCORE;

Movelist moves;
movegen::legalmoves(moves, board);

for (const auto& move : moves) {
  // do something
}

// no moves means game over
if (moves.empty())
  return board.inCheck() ? MATED_SCORE : DRAW_SCORE;
```

## Bitboards

A bitboard stores a set of squares in a 64-bit integer. Bit 0 represents `a1`,
bit 7 represents `h1`, and bit 63 represents `h8`. Use bitwise operations to combine
sets, and iterate over set bits with `pop()` on a nonempty bitboard.
See the [Bitboard reference](/pages/bitboard) for examples and preconditions.
