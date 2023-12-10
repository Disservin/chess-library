#pragma once

#include <cstdint>
#include <bit>
#include <string>
#include <bitset>
#include <algorithm>
#include <iostream>

namespace chess {

class Bitboard {
   public:
    constexpr Bitboard() : bits(0) {}
    constexpr Bitboard(std::uint64_t bits) : bits(bits) {}

    explicit operator bool() const { return bits != 0; }

    explicit operator std::string() const {
        std::bitset<64> b(bits);
        std::string str_bitset = b.to_string();

        std::string str;

        for (int i = 0; i < 64; i += 8) {
            std::string x = str_bitset.substr(i, 8);
            std::reverse(x.begin(), x.end());
            str += x + '\n';
        }
        return str;
    }

    constexpr Bitboard operator&(std::uint64_t rhs) const { return Bitboard(bits & rhs); }
    constexpr Bitboard operator|(std::uint64_t rhs) const { return Bitboard(bits | rhs); }
    constexpr Bitboard operator^(std::uint64_t rhs) const { return Bitboard(bits ^ rhs); }
    constexpr Bitboard operator<<(std::uint64_t rhs) const { return Bitboard(bits << rhs); }
    constexpr Bitboard operator>>(std::uint64_t rhs) const { return Bitboard(bits >> rhs); }
    constexpr bool operator==(std::uint64_t rhs) const { return bits == rhs; }
    constexpr bool operator!=(std::uint64_t rhs) const { return bits != rhs; }

    constexpr Bitboard operator&(const Bitboard& rhs) const { return Bitboard(bits & rhs.bits); }
    constexpr Bitboard operator|(const Bitboard& rhs) const { return Bitboard(bits | rhs.bits); }
    constexpr Bitboard operator^(const Bitboard& rhs) const { return Bitboard(bits ^ rhs.bits); }
    constexpr Bitboard operator~() const { return Bitboard(~bits); }

    constexpr Bitboard& operator&=(const Bitboard& rhs) {
        bits &= rhs.bits;
        return *this;
    }

    constexpr Bitboard& operator|=(const Bitboard& rhs) {
        bits |= rhs.bits;
        return *this;
    }

    constexpr Bitboard& operator^=(const Bitboard& rhs) {
        bits ^= rhs.bits;
        return *this;
    }

    constexpr bool operator==(const Bitboard& rhs) const { return bits == rhs.bits; }
    constexpr bool operator!=(const Bitboard& rhs) const { return bits != rhs.bits; }
    constexpr bool operator||(const Bitboard& rhs) const { return bits || rhs.bits; }
    constexpr bool operator&&(const Bitboard& rhs) const { return bits && rhs.bits; }

    constexpr Bitboard& set(int index) {
        bits |= (1ULL << index);
        return *this;
    }

    constexpr bool check(int index) const { return bits & (1ULL << index); }

    constexpr Bitboard& clear(int index) {
        bits &= ~(1ULL << index);
        return *this;
    }

    constexpr Bitboard& clear() {
        bits = 0;
        return *this;
    }

    constexpr bool empty() const { return bits == 0; }

    constexpr int lsb() const { return std::countr_zero(bits); }
    constexpr int msb() const { return 63 - std::countl_zero(bits); }

    constexpr int count() const { return std::popcount(bits); }

    constexpr std::uint8_t pop() {
        std::uint8_t index = lsb();
        bits &= bits - 1;
        return index;
    }

    constexpr std::uint64_t getBits() const { return bits; }

    friend std::ostream& operator<<(std::ostream& os, const Bitboard& bb);

   private:
    std::uint64_t bits;
};

inline std::ostream& operator<<(std::ostream& os, const Bitboard& bb) {
    os << std::string(bb);
    return os;
}
}  // namespace chess
