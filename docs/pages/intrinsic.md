# Intrinsic Functions

Intrinsic functions are wrapped inside the `builtin` namespace.
They should support all modern OS but lack support for 32 bit archs.

```cpp
namespace builtin {
// returns the index of the least significant bit
int lsb(Bitboard bb);

// returns the index of the most significant bit
int msb(Bitboard bb);

// returns the number of set bits
int popcount(Bitboard bb);

// returns the lsb and pop it
int poplsb(Bitboard &bb);
}
```
