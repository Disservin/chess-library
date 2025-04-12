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

## CheckType

```cpp
enum class CheckType { NO_CHECK, DIRECT_CHECK, DISCOVERY_CHECK };
```
