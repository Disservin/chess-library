# Move Generation

You can generate different types of moves with the `MoveGenType` enum.

```cpp
enum class MoveGenType : uint8_t { ALL, CAPTURE, QUIET };
```

```cpp
class movegen {
    template <MoveGenType mt>
    static void legalmoves(Movelist& movelist, const Board& board , int pieces = 63);
}
```

::: tip
Since 0.4.0 you can pass `pieces` to `legalmoves` to generate moves only for
certain pieces. Combine the `PieceGenType` enum with a bitwise or `|` to generate moves for multiple
piece types.
:::

```cpp
enum PieceGenType {
    PAWN   = 1,
    KNIGHT = 2,
    BISHOP = 4,
    ROOK   = 8,
    QUEEN  = 16,
    KING   = 32,
};
```

::: tip
While `legalmoves<MoveGenType::CAPTURE> + legalmoves<MoveGenType::QUIET> == legalmoves<MoveGenType::ALL>`, it is more efficient to use the latter.
:::
