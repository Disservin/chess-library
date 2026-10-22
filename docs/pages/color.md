# Color

`chess::Color` represents `WHITE` (0), `BLACK` (1), or `NONE` (-1).
A default color is `NONE`. Construct from `Color::WHITE`, `Color::BLACK`, or the
FEN strings `"w"` and `"b"`; unrecognized strings produce `NONE`.

```cpp
chess::Color side = chess::Color::WHITE;
chess::Color opponent = ~side; // Black
```

The `~` operator on a `Color` requires White or Black; do not apply it to `NONE`.
The implicit integer conversion is useful for indexing two-element arrays only
when the color is White or Black. `internal()` returns the underlying enum.

An explicit `std::string` conversion returns `"w"`, `"b"`, or `"NONE"`.
`longStr()` returns `"White"`, `"Black"`, or `"None"`.

See [Piece](/pages/piece) for pieces with colors and [Board](/pages/board) for
queries such as `sideToMove()` and `pieces(type, color)`.
