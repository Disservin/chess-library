#pragma once

#include <cstdint>
#include <bit>

namespace chess {

class Bitboard {
   public:
    constexpr Bitboard() = default;
    constexpr Bitboard(std::uint64_t bits) : bits(bits) {}

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

    constexpr Bitboard& set(std::uint8_t index) {
        bits |= (1ULL << index);
        return *this;
    }

    constexpr Bitboard& clear(std::uint8_t index) {
        bits &= ~(1ULL << index);
        return *this;
    }

    constexpr Bitboard& set(std::uint8_t index, bool value) {
        if (value) {
            set(index);
        } else {
            clear(index);
        }
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

    constexpr Bitboard north() const { return Bitboard(bits << 8); }
    constexpr Bitboard south() const { return Bitboard(bits >> 8); }
    constexpr Bitboard east() const { return Bitboard((bits & 0xfefefefefefefefe) >> 1); }
    constexpr Bitboard west() const { return Bitboard((bits & 0x7f7f7f7f7f7f7f7f) << 1); }

    constexpr Bitboard northEast() const { return Bitboard((bits & 0xfefefefefefefefe) << 7); }
    constexpr Bitboard northWest() const { return Bitboard((bits & 0x7f7f7f7f7f7f7f7f) << 9); }
    constexpr Bitboard southEast() const { return Bitboard((bits & 0xfefefefefefefefe) >> 9); }
    constexpr Bitboard southWest() const { return Bitboard((bits & 0x7f7f7f7f7f7f7f7f) >> 7); }

   private:
    std::uint64_t bits;
};
}  // namespace chess
