# Attacks

If you just want the attacks for a piece on a given square, you
will have to use any of these functions.

```cpp
namespace movegen {
    namespace attacks {
        static constexpr Bitboard pawn(Color c, Square sq);
        static constexpr Bitboard knight(Square sq);
        static constexpr Bitboard bishop(Square sq, Bitboard occ);
        static constexpr Bitboard rook(Square sq, Bitboard occ);
        static constexpr Bitboard queen(Square sq, Bitboard occ);
        static constexpr Bitboard king(Square sq);
    }
}
```
