# Attacks

If you just want the attacks for a piece on a given square, you
will have to use any of these functions.

```cpp
namespace attacks {
    Bitboard pawn(Color c, Square sq);
    Bitboard knight(Square sq);
    Bitboard bishop(Square sq, Bitboard occ);
    Bitboard rook(Square sq, Bitboard occ);
    Bitboard queen(Square sq, Bitboard occ);
    Bitboard king(Square sq);

    template <Color c>
    Bitboard pawnLeftAttacks(const Bitboard pawns);

    template <Color c>
    Bitboard pawnRightAttacks(const Bitboard pawns);

    /// @brief Returns a bitboard with the origin squares of the attacking pieces set
    /// @param board
    /// @param color Attacker Color
    /// @param square Attacked Square
    /// @return
    Bitboard attackers(const Board &board, Color color, Square square);
}
```
