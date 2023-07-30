# Move Generation

You can generate different types of moves with the `MoveGenType` enum.

```cpp
enum class MoveGenType : uint8_t { ALL, CAPTURE, QUIET };
```

```cpp
namespace movegen {
    template <MoveGenType mt>
    void legalmoves(Movelist& movelist, const Board& board);
}
```

::: tip
While `legalmoves<MoveGenType::CAPTURE> + legalmoves<MoveGenType::QUIET> == legalmoves<MoveGenType::ALL>`, it is more efficient to use the latter.
:::
