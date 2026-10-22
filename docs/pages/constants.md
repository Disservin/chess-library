# Constants

All constants below live in `chess::constants`. `MAX_MOVES` is the fixed capacity
of `Movelist`; `STARTPOS` is a standard-chess FEN string.

```cpp
constexpr Bitboard DEFAULT_CHECKMASK = Bitboard(0xFFFFFFFFFFFFFFFFull);
constexpr auto STARTPOS              = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
constexpr auto MAX_MOVES             = 256;
```
