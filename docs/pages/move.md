# The Move Object

Castling moves are encoded as king captures rook. So always make sure the type is not castle
before checking for captures.

## Creation

Incase you want to manually create a move you can do it like so.

```cpp
Move m = Move::make<Move::NORMAL>(Square::underlying::SQ_E2, Square::underlying::SQ_E4);
```

## Other formats

Sometimes you want to convert an internal move object into something else which, like a uci move string or into san notation.
You can easily do this by using the functions inside `uci` namespace.

::: warning
The `moveToSan`, `moveToLan` and `parseSan` functions are relatively new and might produce incorrect results.
Please open an issue for such cases.
:::

## API

```cpp
namespace uci {

/**
 * @brief Converts a move to a UCI string.
 * @param move
 * @param chess960
 * @return
 */
std::string moveToUci(const Move& move, bool chess960 = false);

/**
 * @brief Converts a UCI string to an internal move.
 * @param board
 * @param uci
 * @return NO_MOVE if the move is invalid.
 */
Move uciToMove(const Board& board, const std::string& uci);

/**
 * @brief Converts a move to a SAN string
 * @param board
 * @param move
 * @return
 */
std::string moveToSan(const Board& board, const Move& move);

/**
 * @brief Converts a move to a LAN string
 * @param board
 * @param move
 * @return
 */
std::string moveToLan(const Board& board, const Move& move);

/**
 * @brief Parse a san string and return the move.
 * This function will throw a SanParseError if the san string is invalid.
 * @param board
 * @param san
 * @return
 */
Move parseSan(const Board& board, std::string_view san);

}  // namespace uci
```

```cpp
struct Move {
   public:
    Move() = default;
    Move(uint16_t move);

    template <uint16_t MoveType = 0>
    Move make(
        Square source,
        Square target,
        PieceType pt = PieceType::KNIGHT
    );

    Square from() const;
    Square to() const;
    uint16_t typeOf() const;
    PieceType promotionType() const;

    void setScore(int16_t score) { score_ = score; }

    uint16_t move() const;
    int16_t score() const;

    bool operator==(const Move& rhs) const { return move_ == rhs.move_; }
    bool operator!=(const Move& rhs) const { return move_ != rhs.move_; }

    static constexpr std::uint16_t NO_MOVE   = 0;
    static constexpr std::uint16_t NULL_MOVE = 65;
    static constexpr std::uint16_t NORMAL    = 0;
    static constexpr std::uint16_t PROMOTION = 1 << 14;
    static constexpr std::uint16_t ENPASSANT = 2 << 14;
    static constexpr std::uint16_t CASTLING  = 3 << 14;

    friend std::ostream& operator<<(std::ostream& os, const Move& move);
};
```
