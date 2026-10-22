# Types

Here's a list of all commonly used enums

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

## GameResult

Results are from the current player’s perspective.

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

## CheckType

```cpp
enum class CheckType {
    NO_CHECK,
    CHECK,
    DIRECT_CHECK = CHECK,
    DIRECT_DISCOVERY_CHECK,
    SINGLE_DISCOVERY_CHECK,
    DOUBLE_DISCOVERY_CHECK
};
```

`board.givesCheck(move)` returns `NO_CHECK` or `CHECK`. Use
`board.givesCheck<true>(move)` when you need the more expensive classification
into direct and discovered checks. `DIRECT_CHECK` and `CHECK` have the same value.
Call either overload before making the legal move.
