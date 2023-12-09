#pragma once

#include <cstdint>
#include <string>

namespace chess {

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
        FILE_H
    };

    constexpr File() = default;
    constexpr File(underlying file) : file(file) {}
    constexpr File(char file) : file(static_cast<underlying>(file - 'a')) {}

    constexpr underlying internal() const { return file; }

   private:
    underlying file;
};

class Rank {
   public:
    enum class underlying {
        RANK_1,
        RANK_2,
        RANK_3,
        RANK_4,
        RANK_5,
        RANK_6,
        RANK_7,
        RANK_8,
        NO_RANK
    };

    constexpr Rank() = default;
    constexpr Rank(underlying rank) : rank(rank) {}
    constexpr Rank(char rank) : rank(static_cast<underlying>(rank - '1')) {}

    constexpr underlying internal() const { return rank; }

   private:
    underlying rank;
};

class Square {
   public:
    // clang-format off
    enum class underlying : uint8_t {
        SQ_A1, SQ_B1, SQ_C1, SQ_D1, SQ_E1, SQ_F1, SQ_G1, SQ_H1,
        SQ_A2, SQ_B2, SQ_C2, SQ_D2, SQ_E2, SQ_F2, SQ_G2, SQ_H2,
        SQ_A3, SQ_B3, SQ_C3, SQ_D3, SQ_E3, SQ_F3, SQ_G3, SQ_H3,
        SQ_A4, SQ_B4, SQ_C4, SQ_D4, SQ_E4, SQ_F4, SQ_G4, SQ_H4,
        SQ_A5, SQ_B5, SQ_C5, SQ_D5, SQ_E5, SQ_F5, SQ_G5, SQ_H5,
        SQ_A6, SQ_B6, SQ_C6, SQ_D6, SQ_E6, SQ_F6, SQ_G6, SQ_H6,
        SQ_A7, SQ_B7, SQ_C7, SQ_D7, SQ_E7, SQ_F7, SQ_G7, SQ_H7,
        SQ_A8, SQ_B8, SQ_C8, SQ_D8, SQ_E8, SQ_F8, SQ_G8, SQ_H8,
        NO_SQ
    };
    // clang-format on

    constexpr Square() = default;
    constexpr Square(std::uint8_t sq) : sq(static_cast<underlying>(sq)) {}
    constexpr Square(File file, Rank rank)
        : sq(static_cast<underlying>(static_cast<std::uint8_t>(file.internal()) +
                                     static_cast<std::uint8_t>(rank.internal()) * 8)) {}
    constexpr Square(underlying sq) : sq(sq) {}

    constexpr Square operator^(int i) const {
        return Square(
            static_cast<underlying>(static_cast<std::uint8_t>(sq) ^ static_cast<std::uint8_t>(i)));
    };

    constexpr Square& operator^=(int i) {
        sq = static_cast<underlying>(static_cast<std::uint8_t>(sq) ^ static_cast<std::uint8_t>(i));
        return *this;
    }

    constexpr File file() const { return File(static_cast<std::uint8_t>(sq) & 7); }
    constexpr Rank rank() const { return Rank(static_cast<std::uint8_t>(sq) >> 3); }

    constexpr bool operator==(const Square& rhs) const { return sq == rhs.sq; }
    constexpr bool operator!=(const Square& rhs) const { return sq != rhs.sq; }

    constexpr underlying internal() const { return sq; }

    constexpr bool is_light() const {
        return (static_cast<std::int8_t>(sq) / 8 + static_cast<std::int8_t>(sq) % 8) % 2 == 0;
    }
    constexpr bool is_dark() const { return !is_light(); }

    constexpr bool is_valid() const { return static_cast<std::int8_t>(sq) < 64; }

    static constexpr int max() { return 64; }

    operator std::string() const {
        std::string str;
        str += static_cast<char>(static_cast<int>(file().internal()) + 'a');
        str += static_cast<char>(static_cast<int>(rank().internal()) + '1');
        return str;
    }

   private:
    underlying sq;
};

enum class Direction : int8_t {
    NORTH      = 8,
    WEST       = -1,
    SOUTH      = -8,
    EAST       = 1,
    NORTH_EAST = 9,
    NORTH_WEST = 7,
    SOUTH_WEST = -9,
    SOUTH_EAST = -7
};

}  // namespace chess
