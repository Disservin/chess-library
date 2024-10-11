# Rank

## Description

The `Rank` class is a class that represents a chess rank. It is a simple class that contains an enum class that represents the underlying type of the rank. The class also contains a few static members that represent the underlying type of the rank.

::: tip
Also checkout the [File](/pages/file) class.
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

    constexpr underlying internal() const;

    constexpr bool operator==(const Rank& rhs) const;
    constexpr bool operator!=(const Rank& rhs) const;

    constexpr bool operator==(const underlying& rhs) const;
    constexpr bool operator!=(const underlying& rhs) const;
    constexpr bool operator>=(const Rank& rhs) const;
    constexpr bool operator<=(const Rank& rhs) const;

    operator std::string() const;

    constexpr operator int() const;

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
