# PieceType

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

    explicit operator std::string() const;

    constexpr bool operator==(const PieceType& rhs) const;
    constexpr bool operator!=(const PieceType& rhs) const;
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
