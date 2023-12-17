#pragma once

#include <cstdint>
#include <string>
#include <string_view>

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
        FILE_H,
        NO_FILE
    };

    constexpr File() = default;
    constexpr File(underlying file) : file(file) {}
    constexpr File(char file) : file(static_cast<underlying>(file - 'a')) {}

    constexpr underlying internal() const { return file; }

    constexpr bool operator==(const File& rhs) const { return file == rhs.file; }
    constexpr bool operator!=(const File& rhs) const { return file != rhs.file; }

    constexpr bool operator==(const underlying& rhs) const { return file == rhs; }
    constexpr bool operator!=(const underlying& rhs) const { return file != rhs; }

    constexpr bool operator>=(const File& rhs) const {
        return static_cast<int>(file) >= static_cast<int>(rhs.file);
    }
    constexpr bool operator<=(const File& rhs) const {
        return static_cast<int>(file) <= static_cast<int>(rhs.file);
    }

    static constexpr underlying FILE_A  = underlying::FILE_A;
    static constexpr underlying FILE_B  = underlying::FILE_B;
    static constexpr underlying FILE_C  = underlying::FILE_C;
    static constexpr underlying FILE_D  = underlying::FILE_D;
    static constexpr underlying FILE_E  = underlying::FILE_E;
    static constexpr underlying FILE_F  = underlying::FILE_F;
    static constexpr underlying FILE_G  = underlying::FILE_G;
    static constexpr underlying FILE_H  = underlying::FILE_H;
    static constexpr underlying NO_FILE = underlying::NO_FILE;

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

    constexpr bool operator==(const Rank& rhs) const { return rank == rhs.rank; }
    constexpr bool operator!=(const Rank& rhs) const { return rank != rhs.rank; }

    constexpr bool operator==(const underlying& rhs) const { return rank == rhs; }
    constexpr bool operator!=(const underlying& rhs) const { return rank != rhs; }

    constexpr bool operator>=(const Rank& rhs) const {
        return static_cast<int>(rank) >= static_cast<int>(rhs.rank);
    }
    constexpr bool operator<=(const Rank& rhs) const {
        return static_cast<int>(rank) <= static_cast<int>(rhs.rank);
    }

    static constexpr underlying RANK_1  = underlying::RANK_1;
    static constexpr underlying RANK_2  = underlying::RANK_2;
    static constexpr underlying RANK_3  = underlying::RANK_3;
    static constexpr underlying RANK_4  = underlying::RANK_4;
    static constexpr underlying RANK_5  = underlying::RANK_5;
    static constexpr underlying RANK_6  = underlying::RANK_6;
    static constexpr underlying RANK_7  = underlying::RANK_7;
    static constexpr underlying RANK_8  = underlying::RANK_8;
    static constexpr underlying NO_RANK = underlying::NO_RANK;

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
    constexpr Square(std::string_view str)
        : sq(static_cast<underlying>((str[0] - 'a') + (str[1] - '1') * 8)) {}

    constexpr Square operator^(int i) const {
        return Square(
            static_cast<underlying>(static_cast<std::uint8_t>(sq) ^ static_cast<std::uint8_t>(i)));
    };

    constexpr Square operator^(Square s) const {
        return Square(static_cast<underlying>(static_cast<std::uint8_t>(sq) ^
                                              static_cast<std::uint8_t>(s.internal())));
    };

    constexpr Square& operator^=(int i) {
        sq = static_cast<underlying>(static_cast<std::uint8_t>(sq) ^ static_cast<std::uint8_t>(i));
        return *this;
    }

    constexpr bool operator==(const Square& rhs) const { return sq == rhs.sq; }
    constexpr bool operator!=(const Square& rhs) const { return sq != rhs.sq; }

    operator std::string() const {
        std::string str;
        str += static_cast<char>(static_cast<int>(file().internal()) + 'a');
        str += static_cast<char>(static_cast<int>(rank().internal()) + '1');
        return str;
    }

    constexpr underlying internal() const { return sq; }

    constexpr File file() const { return File(static_cast<std::uint8_t>(sq) & 7); }
    constexpr Rank rank() const { return Rank(static_cast<std::uint8_t>(sq) >> 3); }

    constexpr bool is_light() const {
        return (static_cast<std::int8_t>(sq) / 8 + static_cast<std::int8_t>(sq) % 8) % 2 == 0;
    }
    constexpr bool is_dark() const { return !is_light(); }

    constexpr bool is_valid() const { return static_cast<std::int8_t>(sq) < 64; }

    constexpr int distance(Square sq) const {
        return static_cast<std::int8_t>(std::max(file().internal(), sq.file().internal())) -
               static_cast<std::int8_t>(std::min(file().internal(), sq.file().internal()));
    }

    constexpr int diagonalOf() const {
        return 7 + static_cast<std::int8_t>(rank().internal()) -
               static_cast<std::int8_t>(file().internal());
    }

    constexpr int antiDiagonalOf() const {
        return static_cast<std::int8_t>(rank().internal()) +
               static_cast<std::int8_t>(file().internal());
    }

    static constexpr int max() { return 64; }

   private:
    underlying sq;
};

inline std::ostream& operator<<(std::ostream& os, const Square& sq) {
    os << static_cast<std::string>(sq);
    return os;
}

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
