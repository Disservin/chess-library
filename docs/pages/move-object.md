# The Move Object

Castling moves are encoded as king captures rook. So always make sure the type is not castle
before checking for captures.

## General Implementation

```cpp
struct Move {
   public:
    Move() = default;
    Move(uint16_t move) : move_(move), score_(0) {}

    template <uint16_t MoveType = 0>
    static constexpr Move make(
        Square source,
        Square target,
        PieceType pt = PieceType::KNIGHT
    );

    Square from() const;

    Square to() const;

    uint16_t typeOf() const;

    PieceType promotionType() const;

    constexpr void setScore(int16_t score) { score_ = score; }

    uint16_t move() const;
    int16_t score() const;

    bool operator==(const Move& rhs) const { return move_ == rhs.move_; }
    bool operator!=(const Move& rhs) const { return move_ != rhs.move_; }

    static constexpr uint16_t NO_MOVE = 0;
    static constexpr uint16_t NULL_MOVE = 65;
    static constexpr uint16_t NORMAL = 0;
    static constexpr uint16_t PROMOTION = 1 << 14;
    static constexpr uint16_t ENPASSANT = 2 << 14;
    static constexpr uint16_t CASTLING = 3 << 14;

    friend std::ostream& operator<<(std::ostream& os, const Move& move);

   private:
    uint16_t move_;
    int16_t score_;
};
```

## Creation

Incase you want to manually create a move you can do it like so.

```cpp
Move m = Move::make<Move::NORMAL>(Square::SQ_E2, Square::SQ_E4);
```

## Other formats

Sometimes you want to convert an internal move object into something else which, like a uci move string or into san notation.
You can easily do this by using the functions inside `uci` namespace.

::: warning
The `moveToSan`, `moveToLan` and `parseSan` functions are relatively new and might produce incorrect results.
Please open an issue for such cases.
:::

```cpp
namespace uci {

std::string moveToUci(const Move& move, bool chess960 = false);

Move uciToMove(const Board& board, const std::string& uci);

std::string moveToSan(Board board, const Move& move);

std::string moveToLan(Board board, const Move& move);

Move parseSan(const Board& board, std::string san);

}  // namespace uci
```
