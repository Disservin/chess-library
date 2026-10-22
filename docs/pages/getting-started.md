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

### Board.isGameOver() API

Engines should avoid `isGameOver()` because it redundantly generates legal moves that your engine has already calculated.  
Instead you should use something like this:

```cpp
if (board.isHalfMoveDraw())
  return board.getHalfMoveDrawType().first == GameResultReason::CHECKMATE ? 
         MATED_SCORE : DRAW_SCORE;

if (board.isRepetition())
  return DRAW_SCORE;

Movelist moves;
movegen::legalmoves(moves, board);

for (const auto& move : moves) {
  // do something
}

// no moves means game over
if (moves.empty())
  return inCheck ? MATED_SCORE : DRAW_SCORE;
```

## Understanding Bitboards

A bitboard is a fundamental data structure in chess programming that uses a 64-bit integer to represent a chess board. Each bit corresponds to one square on the board - when a bit is 1, it means something is present on that square (like a piece or an attack). Bitboards make chess operations very efficient since they can use CPU's native bitwise operations.

For example, to represent a white pawn on e2, you would set the bit at position 12 (counting from 0, starting at a1):

```cpp
Bitboard pawn_e2;
pawn_e2.set(12);  // Sets bit for e2 square
```

You can combine bitboards using bitwise operations:

```cpp
Bitboard white_pawns = ...;  // All white pawns
Bitboard black_pawns = ...;  // All black pawns
Bitboard all_pawns = white_pawns | black_pawns;  // Union of both
```

### Bit Position Operations

- `lsb()` - Returns the index of the Least Significant Bit (first 1 bit from right). This is useful for iterating through pieces on a bitboard, as it finds the "lowest" occupied square. This library uses the layout where the first bit is at index 0 (a1), and the last bit is at index 63 (h8).

- `msb()` - Returns the index of the Most Significant Bit (first 1 bit from left). This finds the "highest" occupied square. Rarely used in chess programming.

### Bit Counting and Manipulation

- `count()` - Returns the number of 1 bits in the bitboard. For example, if you have a bitboard representing all pawns, count() would tell you how many pawns are on the board.

- `pop()` - Returns and clears the least significant bit. This is particularly useful when you need to process each piece on a bitboard one at a time. Each call to pop() gives you the next piece position and removes it from the bitboard. This avoids checking every square individually.

- `getBits()` - Returns the raw 64-bit integer underlying the bitboard. This gives you direct access to the underlying 64-bit integer when you need it.
