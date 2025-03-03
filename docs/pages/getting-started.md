# Getting Started

chess-library is a single header library.

Thus to use it in your project just copy the header file `chess.hpp` into your project  
and include in the necessary files.

The entire library is behind the `chess` namespace so for easy usage just say

## Example

```cpp
#include "chess.hpp"

using namespace chess;

```

That's it!

Also see [my examples.](/pages/examples)

::: tip
The library internally uses `assert` and may print debug information to `std::cerr`.
To disable this define `NDEBUG` when compiling.
:::

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

- `pop()` - Returns and clears the least significant bit. This is particularly useful when you need to process each piece on a bitboard one at a time. Each call to pop() gives you the next piece position and removes it from the bitboard. This approach is much faster than any for loop.

- `getBits()` - Returns the raw 64-bit integer underlying the bitboard. This gives you direct access to the underyling 64-bit integer when you need it.
