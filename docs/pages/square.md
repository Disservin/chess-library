# Square

```cpp
class Square {
   public:
    enum class underlying : uint8_t {
        SQ_A1, SQ_B1, SQ_C1, SQ_D1, SQ_E1, SQ_F1, SQ_G1, SQ_H1,
        SQ_A2, SQ_B2, SQ_C2, SQ_D2, SQ_E2, SQ_F2, SQ_G2, SQ_H2,
        SQ_A3, SQ_B3, SQ_C3, SQ_D3, SQ_E3, SQ_F3, SQ_G3, SQ_H3,
        SQ_A4, SQ_B4, SQ_C4, SQ_D4, SQ_E4, SQ_F4, SQ_G4, SQ_H4,
        SQ_A5, SQ_B5, SQ_C5, SQ_D5, SQ_E5, SQ_F5, SQ_G5, SQ_H5,
        SQ_A6, SQ_B6, SQ_C6, SQ_D6, SQ_E6, SQ_F6, SQ_G6, SQ_H6,
        SQ_A7, SQ_B7, SQ_C7, SQ_D7, SQ_E7, SQ_F7, SQ_G7, SQ_H7,
        SQ_A8, SQ_B8, SQ_C8, SQ_D8, SQ_E8, SQ_F8, SQ_G8, SQ_H8,
        NO_SQ
    };

    constexpr Square();
    constexpr Square(int sq);
    constexpr Square(File file, Rank rank);
    constexpr Square(Rank rank, File file);
    constexpr Square(underlying sq);
    constexpr Square(std::string_view str);
    constexpr Square operator^(const Square& s) const;
    constexpr bool operator==(const Square& rhs) const;
    constexpr bool operator!=(const Square& rhs) const;
    constexpr bool operator>(const Square& rhs) const;
    constexpr bool operator>=(const Square& rhs) const;
    constexpr bool operator<(const Square& rhs) const;
    constexpr bool operator<=(const Square& rhs) const;
    constexpr Square operator+(const Square& rhs) const;
    constexpr Square operator-(const Square& rhs) const;
    constexpr Square operator++();
    constexpr Square operator++(int);
    constexpr Square operator--();
    constexpr Square operator--(int);

    operator std::string() const;
    constexpr underlying internal() const;
    constexpr int index() const;
    constexpr File file() const;
    constexpr Rank rank() const;
    constexpr bool is_light() const;
    constexpr bool is_dark() const;
    constexpr bool is_valid() const;
    constexpr static bool is_valid(Rank r, File f);
    static constexpr int distance(Square sq, Square sq2);
    static constexpr int value_distance(Square sq, Square sq2);
    static constexpr bool same_color(Square sq, Square sq2);
    static constexpr bool back_rank(Square sq, Color color);

    /// @brief Flips the square vertically.
    constexpr Square flip();

    /// @brief Conditionally flips the square vertically.
    constexpr Square relative_square(Color c);

    constexpr int diagonal_of();
    constexpr int antidiagonal_of();

    constexpr Square ep_square();
    static constexpr Square castling_king_square(bool is_king_side, Color c);
    static constexpr Square castling_rook_square(bool is_king_side, Color c);
    static constexpr int max();
};
```
