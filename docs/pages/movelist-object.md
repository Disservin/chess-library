# The Movelist Object

```cpp
struct Movelist {
   public:
    void add(Move move);

    /// @brief returns -1 if not found
    int find(Move move);

    int size() const;
    bool empty() const;
    void clear();
    void sort(int index = 0);

    Move operator[](int index) const;
    Move& operator[](int index);
};
```
