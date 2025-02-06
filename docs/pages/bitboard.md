# Bitboard

## Understanding Bitboards

A bitboard is a fundamental data structure in chess programming that uses a 64-bit integer to represent a chess board. Each bit corresponds to one square on the board - when a bit is 1, it means something is present on that square (like a piece or an attack). Bitboards make chess operations very efficient since they can use CPU's native bitwise operations.

For example, to represent a white pawn on e2, you would set the bit at position 12 (counting from 0, starting at a1):

```cpp
Bitboard pawn_e2;
pawn_e2.set(12);  // Sets bit for e2 square
```

You can combine bitboards using bitwise operations:

```cpp
Bitboard white_pawns = ...;  // All white pawns
Bitboard black_pawns = ...;  // All black pawns
Bitboard all_pawns = white_pawns | black_pawns;  // Union of both
```

### Bit Position Operations

- `lsb()` - Returns the index of the Least Significant Bit (first 1 bit from right). This is useful for iterating through pieces on a bitboard, as it finds the "lowest" occupied square. This library uses the layout where the first bit is at index 0 (a1), and the last bit is at index 63 (h8).

- `msb()` - Returns the index of the Most Significant Bit (first 1 bit from left). This finds the "highest" occupied square. Rarely used in chess programming.

### Bit Counting and Manipulation

- `count()` - Returns the number of 1 bits in the bitboard. For example, if you have a bitboard representing all pawns, count() would tell you how many pawns are on the board.

- `pop()` - Returns and clears the least significant bit. This is particularly useful when you need to process each piece on a bitboard one at a time. Each call to pop() gives you the next piece position and removes it from the bitboard. This approach is much faster than any for loop.

- `getBits()` - Returns the raw 64-bit integer underlying the bitboard. This gives you direct access to the underyling 64-bit integer when you need it.

## API

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
