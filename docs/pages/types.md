# Types

Here's a list of all commonly used enums

## MoveGenType

```cpp
enum class MoveGenType : uint8_t { ALL, CAPTURE, QUIET };
```

## Square

```cpp
enum Square : uint8_t {
    SQ_A1, SQ_B1, SQ_C1, SQ_D1, SQ_E1, SQ_F1, SQ_G1, SQ_H1,
    SQ_A2, SQ_B2, SQ_C2, SQ_D2, SQ_E2, SQ_F2, SQ_G2, SQ_H2,
    SQ_A3, SQ_B3, SQ_C3, SQ_D3, SQ_E3, SQ_F3, SQ_G3, SQ_H3,
    SQ_A4, SQ_B4, SQ_C4, SQ_D4, SQ_E4, SQ_F4, SQ_G4, SQ_H4,
    SQ_A5, SQ_B5, SQ_C5, SQ_D5, SQ_E5, SQ_F5, SQ_G5, SQ_H5,
    SQ_A6, SQ_B6, SQ_C6, SQ_D6, SQ_E6, SQ_F6, SQ_G6, SQ_H6,
    SQ_A7, SQ_B7, SQ_C7, SQ_D7, SQ_E7, SQ_F7, SQ_G7, SQ_H7,
    SQ_A8, SQ_B8, SQ_C8, SQ_D8, SQ_E8, SQ_F8, SQ_G8, SQ_H8,
    NO_SQ
};
```

## Piece

```cpp
enum class Piece : uint8_t {
    WHITEPAWN,
    WHITEKNIGHT,
    WHITEBISHOP,
    WHITEROOK,
    WHITEQUEEN,
    WHITEKING,
    BLACKPAWN,
    BLACKKNIGHT,
    BLACKBISHOP,
    BLACKROOK,
    BLACKQUEEN,
    BLACKKING,
    NONE
};
```

## PieceType

```cpp
enum class PieceType : uint8_t {
    PAWN,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING,
    NONE
};

```

## Rank

```cpp
enum class Rank {
    RANK_1,
    RANK_2,
    RANK_3,
    RANK_4,
    RANK_5,
    RANK_6,
    RANK_7,
    RANK_8,
    NO_RANK
};

```

## File

```cpp
enum class File {
    NO_FILE = -1,
    FILE_A,
    FILE_B,
    FILE_C,
    FILE_D,
    FILE_E,
    FILE_F,
    FILE_G,
    FILE_H
};

```

## CastleSide

```cpp
enum class CastleSide : uint8_t { KING_SIDE, QUEEN_SIDE };

```

## Direction

```cpp
enum class Direction : int8_t {
    NORTH = 8,
    WEST = -1,
    SOUTH = -8,
    EAST = 1,
    NORTH_EAST = 9,
    NORTH_WEST = 7,
    SOUTH_WEST = -9,
    SOUTH_EAST = -7
};
```

## Color

```cpp
enum class Color : uint8_t { WHITE, BLACK, NONE };
```

## GameResult

Results are from the current players perspective.

```cpp
enum class GameResult { WIN, LOSE, DRAW, NONE };
```

## GameResultReason

You have access to this information when calling `isGameOver()`.  
`NONE` simply means that the game is not over yet.

```cpp
enum class GameResultReason {
    CHECKMATE,
    STALEMATE,
    INSUFFICIENT_MATERIAL,
    FIFTY_MOVE_RULE,
    THREEFOLD_REPETITION,
    NONE
};
```

## Operator Overloads

::: danger
The usage of `enum classes` provides more type safety compared to plain enums because they prohibit implicit conversion to integers.

If you know what you are doing, you can make use of the `ADD_BASE_OPERATORS_FOR` and `ADD_INCR_OPERATORS_FOR` macros. Which will overload the enum class with the following operators.
:::

| ADD_BASE_OPERATORS_FOR | ADD_INCR_OPERATORS_FOR |
| ---------------------- | ---------------------- |
| +                      | ++ _prefix_            |
| -                      | -- _prefix_            |
| -                      | ++ _postfix_           |
| +=                     | -- _postfix_           |
| -=                     |                        |

### Usage

```cpp
ADD_INCR_OPERATORS_FOR(Piece)
```

::: tip
Only `Square` is currenlty implemented without an enum class. So you dont need to overload any operators for this.
:::
