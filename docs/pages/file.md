# File

## Description

Files run from `a` to `h`, with integer values 0–7. The default is
`File::NO_FILE` (8). Construct from a valid one-character string such as `"e"`,
or use `File::FILE_E`. String conversion returns the lowercase file letter.

::: tip
Also see the [Rank](/pages/rank) class.
:::

## API

```cpp
class File {
   public:
    enum class underlying : std::uint8_t { 
        FILE_A,
        FILE_B,
        FILE_C,
        FILE_D,
        FILE_E,
        FILE_F,
        FILE_G,
        FILE_H,
        NO_FILE 
    };

    constexpr File();
    constexpr File(underlying file);
    constexpr File(int file);
    constexpr File(std::string_view str);
    constexpr File& operator+=(int rhs);
    constexpr underlying internal() const;
    constexpr bool operator==(const File& rhs) const;
    constexpr bool operator!=(const File& rhs) const;
    constexpr bool operator==(const underlying& rhs) const;
    constexpr bool operator!=(const underlying& rhs) const;
    constexpr bool operator>=(const File& rhs) const;
    constexpr bool operator<=(const File& rhs) const;
    constexpr bool operator>(const File& rhs) const;
    constexpr bool operator<(const File& rhs) const;

    constexpr operator int() const;
    explicit operator std::string() const;

    static constexpr underlying FILE_A  = underlying::FILE_A;
    static constexpr underlying FILE_B  = underlying::FILE_B;
    static constexpr underlying FILE_C  = underlying::FILE_C;
    static constexpr underlying FILE_D  = underlying::FILE_D;
    static constexpr underlying FILE_E  = underlying::FILE_E;
    static constexpr underlying FILE_F  = underlying::FILE_F;
    static constexpr underlying FILE_G  = underlying::FILE_G;
    static constexpr underlying FILE_H  = underlying::FILE_H;
    static constexpr underlying NO_FILE = underlying::NO_FILE;
};
```
