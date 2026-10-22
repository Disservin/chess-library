# The Move Object

Castling moves are encoded as king captures rook. So always make sure the type is not castle
before checking for captures.

## Creation

If you want to manually create a move you can do it like so.

```cpp
Move m = Move::make<Move::NORMAL>(Square::SQ_E2, Square::SQ_E4);
```

## Other formats

Use the static functions on `chess::uci` to convert between internal moves,
UCI coordinate notation (`e2e4`, `e7e8q`), SAN (`e4`, `Nf3`, `O-O`), and LAN.
SAN/LAN output and SAN parsing need the board **before** the move.

```cpp
chess::Board board;
const auto move = chess::uci::parseSan(board, "e4");
const auto san = chess::uci::moveToSan(board, move);
const auto uci = chess::uci::moveToUci(move, board.chess960());
board.makeMove(move);
```

`uciToMove` decodes a move using the board to identify castling, promotions, and
en passant. It returns `Move::NO_MOVE` for some malformed inputs, but does not
check legality. For external UCI input, check `uci::isUciMove(text)` first, decode
it, then reject `Move::NO_MOVE` or a move that fails `board.isLegal(move)`.
`isUciMove` only checks coordinate syntax, not the board position.

`parseSan` matches against generated legal moves. With exceptions enabled,
unmatched SAN throws `chess::uci::SanParseError` and ambiguous SAN throws
`chess::uci::AmbiguousMoveError`; an empty string returns
`Move::NO_MOVE`. With `CHESS_NO_EXCEPTIONS`, unmatched SAN returns `Move::NO_MOVE`, but
ambiguous SAN can return a matching move without reporting the ambiguity.
It accepts some relaxed notation and is not a strict SAN syntax validator.
The overload `parseSan(board, san, scratchMoves)` lets you reuse a `Movelist`;
it replaces that list’s contents during parsing of nonempty SAN.

For standard castling, UCI output uses the king’s destination (`e1g1`). Passing
`true` to `moveToUci` uses the internal king-to-rook convention required by
Chess960. Decode Chess960 UCI input with a board configured for Chess960.

## API

```cpp
class uci {
public:

/**
 * @brief Converts a move to a UCI string.
 * @param move
 * @param chess960
 * @return
 */
static std::string moveToUci(const Move& move, bool chess960 = false);

/**
 * @brief Converts a UCI string to an internal move.
 * @param board
 * @param uci
 * @return NO_MOVE for some malformed inputs; legality must be checked separately.
 */
static Move uciToMove(const Board& board, std::string_view uci);

/**
 * @brief Converts a move to a SAN string
 * @param board
 * @param move
 * @return
 */
static std::string moveToSan(const Board& board, const Move& move);

/**
 * @brief Converts a move to a LAN string
 * @param board
 * @param move
 * @return
 */
static std::string moveToLan(const Board& board, const Move& move);

/**
 * @brief Parse a san string and return the move.
 * This function will throw a SanParseError if the san string is invalid.
 * @param board
 * @param san
 * @return
 */
static Move parseSan(const Board& board, std::string_view san);

/**
 * @brief Check if a string is a valid UCI move. Must also have the correct length.
 * @param move
 * @return
 */
static bool isUciMove(std::string_view move) noexcept;
};
```

```cpp
class Move {
   public:
    Move() = default;
    constexpr Move(uint16_t move);

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

    void setScore(int16_t score);

    uint16_t move() const;
    int16_t score() const;

    bool operator==(const Move& rhs) const;
    bool operator!=(const Move& rhs) const;

    static constexpr std::uint16_t NO_MOVE   = 0;
    static constexpr std::uint16_t NULL_MOVE = 65;
    static constexpr std::uint16_t NORMAL    = 0;
    static constexpr std::uint16_t PROMOTION = 1 << 14;
    static constexpr std::uint16_t ENPASSANT = 2 << 14;
    static constexpr std::uint16_t CASTLING  = 3 << 14;
};
```

## Move values and scores

`Move::make` encodes a move; it does not check the board. Select the correct
`NORMAL`, `PROMOTION`, `ENPASSANT`, or `CASTLING` type. Only call `promotionType()`
for a promotion. `Move::make<Move::PROMOTION>(from, to, PieceType::QUEEN)` encodes
a queen promotion.

Use `board.isCapture(move)` and `board.getCapturing(move)` before making a move
to handle en passant and castling correctly. Despite its name, `getCapturing`
returns the **captured** piece (or `Piece::NONE`), not the moving piece.

`setScore()` stores a signed 16-bit ordering score. Move equality compares only
the encoded move, ignoring its score. A default-constructed `Move` is uninitialized;
use `Move move = Move::NO_MOVE` when you need a sentinel. Null moves use
`board.makeNullMove()` and `board.unmakeNullMove()`, not `makeMove(NULL_MOVE)`.
