# Piece

## Description

`Piece` combines a piece type and color. A default piece is `Piece::NONE`,
representing an empty square. `Piece("P")` is a white pawn; `Piece("p")` is a black
pawn. String construction reads the first character, so supply a nonempty string.
Use `type()` and `color()` to inspect it, and check for `NONE` before using the
color as an index or taking its opposite. String conversion returns the FEN letter
(or `"."` for an empty square).

::: tip
Also see the [PieceType](/pages/piece-type) class.
:::

## API

```cpp
class Piece {
   public:
    enum class underlying : std::uint8_t {
        WHITEPAWN,
        WHITEKNIGHT,
        WHITEBISHOP,
        WHITEROOK,
        WHITEQUEEN,
        WHITEKING,
        BLACKPAWN,
        BLACKKNIGHT,
        BLACKBISHOP,
        BLACKROOK,
        BLACKQUEEN,
        BLACKKING,
        NONE
    };

    constexpr Piece();
    constexpr Piece(underlying piece);
    constexpr Piece(PieceType type, Color color);
    constexpr Piece(Color color, PieceType type);
    constexpr Piece(std::string_view p);
    constexpr bool operator<(const Piece& rhs) const;
    constexpr bool operator>(const Piece& rhs) const;
    constexpr bool operator==(const Piece& rhs) const;
    constexpr bool operator!=(const Piece& rhs) const;

    constexpr bool operator==(const underlying& rhs) const;
    constexpr bool operator!=(const underlying& rhs) const;

    constexpr bool operator==(const PieceType& rhs) const;
    constexpr bool operator!=(const PieceType& rhs) const;
    explicit operator std::string() const;
    explicit operator std::string_view() const;
    constexpr operator int() const;

    constexpr PieceType type() const;

    constexpr Color color() const;

    constexpr underlying internal() const;

    static constexpr underlying NONE        = underlying::NONE;
    static constexpr underlying WHITEPAWN   = underlying::WHITEPAWN;
    static constexpr underlying WHITEKNIGHT = underlying::WHITEKNIGHT;
    static constexpr underlying WHITEBISHOP = underlying::WHITEBISHOP;
    static constexpr underlying WHITEROOK   = underlying::WHITEROOK;
    static constexpr underlying WHITEQUEEN  = underlying::WHITEQUEEN;
    static constexpr underlying WHITEKING   = underlying::WHITEKING;
    static constexpr underlying BLACKPAWN   = underlying::BLACKPAWN;
    static constexpr underlying BLACKKNIGHT = underlying::BLACKKNIGHT;
    static constexpr underlying BLACKBISHOP = underlying::BLACKBISHOP;
    static constexpr underlying BLACKROOK   = underlying::BLACKROOK;
    static constexpr underlying BLACKQUEEN  = underlying::BLACKQUEEN;
    static constexpr underlying BLACKKING   = underlying::BLACKKING;
};
```
