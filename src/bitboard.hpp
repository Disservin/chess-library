#pragma once

#include <cstdint>
#if __cplusplus >= 202002L
#include <bit>
#endif
#include <string>
#include <bitset>
#include <algorithm>
#include <iostream>
#include <cassert>

#if defined(_MSC_VER)
#include <intrin.h>
#include <nmmintrin.h>
#endif

#include "coords.hpp"

namespace chess {

class Bitboard {
   public:
    constexpr Bitboard() : bits(0) {}
    constexpr Bitboard(std::uint64_t bits) : bits(bits) {}
    constexpr Bitboard(File file) : bits(0) {
        assert(file != File::NO_FILE);
        bits = 0x0101010101010101ULL << static_cast<int>(file.internal());
    }
    constexpr Bitboard(Rank rank) : bits(0) {
        assert(rank != Rank::NO_RANK);
        bits = 0xFFULL << (8 * static_cast<int>(rank.internal()));
    }

    explicit operator bool() const noexcept { return bits != 0; }

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

    constexpr Bitboard operator&(std::uint64_t rhs) const noexcept { return Bitboard(bits & rhs); }
    constexpr Bitboard operator|(std::uint64_t rhs) const noexcept { return Bitboard(bits | rhs); }
    constexpr Bitboard operator^(std::uint64_t rhs) const noexcept { return Bitboard(bits ^ rhs); }
    constexpr Bitboard operator<<(std::uint64_t rhs) const noexcept { return Bitboard(bits << rhs); }
    constexpr Bitboard operator>>(std::uint64_t rhs) const noexcept { return Bitboard(bits >> rhs); }
    constexpr bool operator==(std::uint64_t rhs) const noexcept { return bits == rhs; }
    constexpr bool operator!=(std::uint64_t rhs) const noexcept { return bits != rhs; }

    constexpr Bitboard operator&(const Bitboard& rhs) const noexcept { return Bitboard(bits & rhs.bits); }
    constexpr Bitboard operator|(const Bitboard& rhs) const noexcept { return Bitboard(bits | rhs.bits); }
    constexpr Bitboard operator^(const Bitboard& rhs) const noexcept { return Bitboard(bits ^ rhs.bits); }
    constexpr Bitboard operator~() const noexcept { return Bitboard(~bits); }

    constexpr Bitboard& operator&=(const Bitboard& rhs) noexcept {
        bits &= rhs.bits;
        return *this;
    }

    constexpr Bitboard& operator|=(const Bitboard& rhs) noexcept {
        bits |= rhs.bits;
        return *this;
    }

    constexpr Bitboard& operator^=(const Bitboard& rhs) noexcept {
        bits ^= rhs.bits;
        return *this;
    }

    constexpr bool operator==(const Bitboard& rhs) const noexcept { return bits == rhs.bits; }
    constexpr bool operator!=(const Bitboard& rhs) const noexcept { return bits != rhs.bits; }
    constexpr bool operator||(const Bitboard& rhs) const noexcept { return bits || rhs.bits; }
    constexpr bool operator&&(const Bitboard& rhs) const noexcept { return bits && rhs.bits; }

    constexpr Bitboard& set(int index) noexcept {
        assert(index >= 0 && index < 64);
        bits |= (1ULL << index);
        return *this;
    }

    [[nodiscard]] constexpr bool check(int index) const noexcept {
        assert(index >= 0 && index < 64);
        return bits & (1ULL << index);
    }

    constexpr Bitboard& clear(int index) noexcept {
        assert(index >= 0 && index < 64);
        bits &= ~(1ULL << index);
        return *this;
    }

    constexpr Bitboard& clear() noexcept {
        bits = 0;
        return *this;
    }

    [[nodiscard]] static constexpr Bitboard fromSquare(int index) noexcept {
        assert(index >= 0 && index < 64);
        return Bitboard(1ULL << index);
    }

    [[nodiscard]] static constexpr Bitboard fromSquare(Square sq) noexcept {
        assert(sq.index() >= 0 && sq.index() < 64);
        return Bitboard(1ULL << sq.index());
    }

    [[nodiscard]] constexpr bool empty() const noexcept { return bits == 0; }

    [[nodiscard]]
#if !defined(_MSC_VER)
    constexpr
#endif
        int
        lsb() const noexcept {
        assert(bits != 0);
#if __cplusplus >= 202002L
        return std::countr_zero(bits);
#else
#if defined(__GNUC__)
        return __builtin_ctzll(bits);
#elif defined(_MSC_VER)
        unsigned long idx;
        _BitScanForward64(&idx, bits);
        return static_cast<int>(idx);
#else
#error "Compiler not supported."
#endif
#endif
    }

    [[nodiscard]]
#if !defined(_MSC_VER)
    constexpr
#endif
        int
        msb() const noexcept {
        assert(bits != 0);

#if __cplusplus >= 202002L
        return std::countl_zero(bits) ^ 63;
#else
#if defined(__GNUC__)
        return 63 ^ __builtin_clzll(bits);
#elif defined(_MSC_VER)
        unsigned long idx;
        _BitScanReverse64(&idx, bits);
        return static_cast<int>(idx);
#else
#error "Compiler not supported."
#endif
#endif
    }

    [[nodiscard]]
#if !defined(_MSC_VER)
    constexpr
#endif
        int
        count() const noexcept {
#if __cplusplus >= 202002L
        return std::popcount(bits);
#else
#if defined(_MSC_VER) || defined(__INTEL_COMPILER)
        return static_cast<int>(_mm_popcnt_u64(bits));
#else
        return __builtin_popcountll(bits);
#endif
#endif
    }

    [[nodiscard]]
#if !defined(_MSC_VER)
    constexpr
#endif
        std::uint8_t
        pop() noexcept {
        assert(bits != 0);
        std::uint8_t index = lsb();
        bits &= bits - 1;
        return index;
    }

    [[nodiscard]] constexpr std::uint64_t getBits() const noexcept { return bits; }

    friend std::ostream& operator<<(std::ostream& os, const Bitboard& bb);

   private:
    std::uint64_t bits;
};

inline std::ostream& operator<<(std::ostream& os, const Bitboard& bb) {
    os << std::string(bb);
    return os;
}

constexpr Bitboard operator&(std::uint64_t lhs, const Bitboard& rhs) { return rhs & lhs; }
constexpr Bitboard operator|(std::uint64_t lhs, const Bitboard& rhs) { return rhs | lhs; }
}  // namespace chess
