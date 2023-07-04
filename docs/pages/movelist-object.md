# The Movelist Object

```cpp
struct Movelist {
   public:
    constexpr void add(Move move);

    // returns -1 if not found
    constexpr int find(Move move);

    constexpr int size() const;
    constexpr bool empty() const;
    constexpr void clear();
    inline void sort(int index = 0);

    constexpr Move operator[](int index) const;
    constexpr Move& operator[](int index);

   private:
    Move moves_[MAX_MOVES];
    int size_ = 0;
};
```
