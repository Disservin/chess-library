# Attacks

`chess::attacks` is a class of static helpers for geometric attacks. These are
not legal moves: they do not filter pins, check evasions, or friendly occupied
squares. Pawn attacks are diagonal captures, not forward pawn pushes.

For sliding pieces, pass the full occupancy (`board.occ()`). Rays include the
first occupied square and stop there. Remove friendly squares from the result if
needed. `attackers(board, color, target)` returns origin squares of attackers of
that color, rather than their destinations. A pinned piece can still appear.

`shift<Direction::NORTH>(bits)` and the other directions shift a bitboard while
masking file edges. `slider<PieceType::BISHOP/ROOK/QUEEN>` selects a sliding attack
at compile time. Attack tables initialize automatically; applications do not need
to call `initAttacks()`.

## API

```cpp
class attacks {
public:
    template <Direction direction>
    static constexpr Bitboard shift(Bitboard b);

    template <PieceType::underlying pt>
    static Bitboard slider(Square sq, Bitboard occ);

    static Bitboard pawn(Color c, Square sq);
    static Bitboard knight(Square sq);
    static Bitboard bishop(Square sq, Bitboard occ);
    static Bitboard rook(Square sq, Bitboard occ);
    static Bitboard queen(Square sq, Bitboard occ);
    static Bitboard king(Square sq);

    template <Color::underlying c>
    static Bitboard pawnLeftAttacks(const Bitboard pawns);

    template <Color::underlying c>
    static Bitboard pawnRightAttacks(const Bitboard pawns);

    /// @brief Returns the origin squares of pieces of a given color attacking a target square
    /// @param board
    /// @param color Attacker Color
    /// @param square Attacked Square
    /// @return
    static Bitboard attackers(const Board &board, Color color, Square square);
};
```
