# Rank

## Description

Ranks run from `1` to `8`, with integer values 0–7. The default is
`Rank::NO_RANK` (8). Construct from a valid one-character string such as `"2"`,
or use `Rank::RANK_2`. `bb()` returns the rank’s 64-bit mask.
`Rank::rank(rank, color)` reflects the rank for Black and preserves it for White.

::: tip
Also see the [File](/pages/file) class.
:::

## API

```cpp

class Rank {
   public:
    enum class underlying {
        RANK_1,
        RANK_2,
        RANK_3,
        RANK_4,
        RANK_5,
        RANK_6,
        RANK_7,
        RANK_8,
        NO_RANK
    };

    constexpr Rank();
    constexpr Rank(underlying rank);
    constexpr Rank(int rank);
    constexpr Rank(std::string_view str);
    constexpr Rank& operator+=(int rhs);

    constexpr underlying internal() const;

    constexpr bool operator==(const Rank& rhs) const;
    constexpr bool operator!=(const Rank& rhs) const;

    constexpr bool operator==(const underlying& rhs) const;
    constexpr bool operator!=(const underlying& rhs) const;
    constexpr bool operator>=(const Rank& rhs) const;
    constexpr bool operator<=(const Rank& rhs) const;

    operator std::string() const;

    constexpr operator int() const;

    constexpr std::uint64_t bb() const;
    static constexpr Rank rank(Rank rank, Color color);

    static constexpr bool back_rank(Rank r, Color color);

    static constexpr underlying RANK_1  = underlying::RANK_1;
    static constexpr underlying RANK_2  = underlying::RANK_2;
    static constexpr underlying RANK_3  = underlying::RANK_3;
    static constexpr underlying RANK_4  = underlying::RANK_4;
    static constexpr underlying RANK_5  = underlying::RANK_5;
    static constexpr underlying RANK_6  = underlying::RANK_6;
    static constexpr underlying RANK_7  = underlying::RANK_7;
    static constexpr underlying RANK_8  = underlying::RANK_8;
    static constexpr underlying NO_RANK = underlying::NO_RANK;
};
```
