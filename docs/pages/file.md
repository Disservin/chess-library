# File

## Description

The `File` class is a class that represents a chess file. It is a simple class that contains an enum class that represents the underlying type of the file. The class also contains a few static members that represent the underlying type of the file.

::: tip
Also checkout the [Rank](/pages/rank) class.
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
