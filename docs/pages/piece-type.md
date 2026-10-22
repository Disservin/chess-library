# PieceType

## Description

`PieceType` represents a pawn, knight, bishop, rook, queen, or king without a
color. The values are 0–5; `NONE` is 6 and is the default. Construct it from a
piece-type letter such as `"n"` (case-insensitive), or use `PieceType::KNIGHT`.
String construction requires a nonempty string. String conversion returns a
lowercase letter, or a space for `NONE`.

::: tip
Also see the [Piece](/pages/piece) class.
:::

## API

```cpp
class PieceType {
   public:
    enum class underlying : std::uint8_t {
        PAWN,
        KNIGHT,
        BISHOP,
        ROOK,
        QUEEN,
        KING,
        NONE,
    };

    constexpr PieceType();
    constexpr PieceType(underlying pt);
    constexpr explicit PieceType(std::string_view type);

    constexpr bool operator==(const PieceType& rhs) const;
    constexpr bool operator!=(const PieceType& rhs) const;

    explicit operator std::string() const;
    constexpr operator int() const;

    constexpr underlying internal() const;

    static constexpr underlying PAWN   = underlying::PAWN;
    static constexpr underlying KNIGHT = underlying::KNIGHT;
    static constexpr underlying BISHOP = underlying::BISHOP;
    static constexpr underlying ROOK   = underlying::ROOK;
    static constexpr underlying QUEEN  = underlying::QUEEN;
    static constexpr underlying KING   = underlying::KING;
    static constexpr underlying NONE   = underlying::NONE;
};

```
