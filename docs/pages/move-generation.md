# Move generation

`chess::movegen::legalmoves` generates legal moves for the board’s side to move,
including evasions when in check, promotions, en passant, and castling when legal.
It clears the supplied `Movelist` before filling it. Generation does not change
the board and does not stop because a repetition or fifty-move draw is available.

```cpp
chess::Board board;
chess::Movelist moves;
chess::movegen::legalmoves(moves, board);

chess::movegen::legalmoves<chess::movegen::MoveGenType::CAPTURE>(moves, board);
chess::movegen::legalmoves<chess::movegen::MoveGenType::QUIET>(moves, board);

// Restrict generation to knights and bishops.
chess::movegen::legalmoves(moves, board, chess::KNIGHT | chess::BISHOP);

// Stop as soon as a legal move is found.
bool hasMove = chess::movegen::anylegalmoves(board);
```

## Generation filters

`MoveGenType` is nested in `movegen`:

```cpp
enum class MoveGenType : std::uint8_t { ALL, CAPTURE, QUIET };
```

`ALL` is the default. `CAPTURE` includes en passant; `QUIET` includes non-captures,
including castling and non-capturing promotions. Generating `ALL` once is more
efficient than generating captures and quiet moves separately when both are needed.

The `pieces` argument is a bit mask of `chess::PieceGenType` values:

```cpp
enum PieceGenType {
    PAWN = 1, KNIGHT = 2, BISHOP = 4, ROOK = 8, QUEEN = 16, KING = 32
};
```

The default mask is 63 (all piece types). `anylegalmoves` accepts the same move
and piece filters. A false result with a restricted filter does not imply that
the position is checkmate or stalemate.

## Check a specific move

`movegen::isLegal(board, move)` and `board.isLegal(move)` check a move against the
current position. They assume a structurally valid move encoding, such as a valid
promotion piece. `Board::makeMove` requires a legal move and does not validate it.
See [move formats](/pages/move#other-formats) for converting external move strings.
