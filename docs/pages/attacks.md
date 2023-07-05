# Attacks

If you just want the attacks for a piece on a given square, you
will have to use any of these functions.

```cpp
namespace movegen {
    namespace attacks {
        Bitboard pawn(Color c, Square sq);
        Bitboard knight(Square sq);
        Bitboard bishop(Square sq, Bitboard occ);
        Bitboard rook(Square sq, Bitboard occ);
        Bitboard queen(Square sq, Bitboard occ);
        Bitboard king(Square sq);
    }
}
```
