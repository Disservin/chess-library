# The Movelist Object

::: tip
Also checkout the [Move](/pages/move) class.
:::

## API

```cpp
struct Movelist {
   public:
    // Throws std::out_of_range if pos is not within the range of the movelist.
    constexpr Move& at(size_type pos);
    constexpr const Move& at(size_type pos) const;

    constexpr Move& operator[](size_type pos) noexcept;
    constexpr const Move& operator[](size_type pos) const noexcept;

    constexpr Move& front() noexcept;
    constexpr const Move& front() const noexcept;

    constexpr Move& back() noexcept;
    constexpr const Move& back() const noexcept;

    // Iterators

    constexpr Move* begin() noexcept;
    constexpr const Move* begin() const noexcept;

    constexpr Move* end() noexcept;
    constexpr const Move* end() const noexcept;

    // Capacity

    /// @brief Checks if the movelist is empty.
    /// @return
    constexpr bool empty() const noexcept;

    /// @brief Return the number of moves in the movelist.
    /// @return
    constexpr int size() const noexcept;

    // Modifiers

    /// @brief Clears the movelist.
    constexpr void clear() noexcept;

    /// @brief Add a move to the end of the movelist.
    /// @param move
    constexpr void add(const Move& move) noexcept;

    /// @brief Add a move to the end of the movelist.
    /// @param move
    constexpr void add(value_type&& move) noexcept;

    // Other

    /// @brief Checks if a move is in the movelist, returns the index of the move if it is found,
    /// otherwise -1.
    /// @param move
    /// @return
    [[deprecated("Use std::find() instead.")]] constexpr size_type find(value_type move) const noexcept;

};
```
