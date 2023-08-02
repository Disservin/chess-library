# Move Generation

You can generate different types of moves with the `MoveGenType` enum.

```cpp
enum class MoveGenType : uint8_t { ALL, NOISY, QUIET };
```

Noisy generates, captures, enpassants and promotions. Quiet generates all other moves. All generates all moves.
The board has functions to check if a move is noisy or a capture.

````cpp

```cpp
namespace movegen {
    template <MoveGenType mt>
    void legalmoves(Movelist& movelist, const Board& board);
}
````

::: tip
While `legalmoves<MoveGenType::NOISY> + legalmoves<MoveGenType::QUIET> == legalmoves<MoveGenType::ALL>`, it is more efficient to use the latter.
:::
