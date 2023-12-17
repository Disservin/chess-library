# Bitboard

```cpp
class Bitboard {
   public:
    constexpr Bitboard();
    constexpr Bitboard(std::uint64_t bits);
    constexpr Bitboard(File file);
    constexpr Bitboard(Rank rank);

    explicit operator bool() const;
    explicit operator std::string() const;

    constexpr Bitboard operator&(std::uint64_t rhs) const;
    constexpr Bitboard operator|(std::uint64_t rhs) const;
    constexpr Bitboard operator^(std::uint64_t rhs) const;
    constexpr Bitboard operator<<(std::uint64_t rhs) const;
    constexpr Bitboard operator>>(std::uint64_t rhs) const;
    constexpr Bitboard operator&(const Bitboard& rhs) const;
    constexpr Bitboard operator|(const Bitboard& rhs) const;
    constexpr Bitboard operator^(const Bitboard& rhs) const;
    constexpr Bitboard operator~() const;
    constexpr Bitboard& operator&=(const Bitboard& rhs);
    constexpr Bitboard& operator|=(const Bitboard& rhs);
    constexpr Bitboard& operator^=(const Bitboard& rhs);

    constexpr bool operator==(const Bitboard& rhs) const;
    constexpr bool operator!=(const Bitboard& rhs) const;
    constexpr bool operator||(const Bitboard& rhs) const;
    constexpr bool operator&&(const Bitboard& rhs) const;

    constexpr bool operator==(std::uint64_t rhs) const;
    constexpr bool operator!=(std::uint64_t rhs) const;

    constexpr Bitboard& set(int index);
    constexpr bool check(int index) const;

    constexpr Bitboard& clear(int index);
    constexpr Bitboard& clear();

    static constexpr Bitboard fromSquare(int index);
    static constexpr Bitboard fromSquare(Square sq);

    constexpr bool empty() const;
    
    constexpr int lsb() const;
    constexpr int msb() const;
    constexpr int count() const;
    constexpr std::uint8_t pop();

    constexpr std::uint64_t getBits() const;

    friend std::ostream& operator<<(std::ostream& os, const Bitboard& bb);
};
```
