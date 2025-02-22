#pragma once

#include <cassert>
#include <cstdint>
#include <string>
#include <string_view>

#include "color.hpp"
#include "utils.hpp"

namespace chess {

#define CHESS_DECLARE_RANK(N)                            \
    static constexpr auto SQ_A##N = underlying::SQ_A##N; \
    static constexpr auto SQ_B##N = underlying::SQ_B##N; \
    static constexpr auto SQ_C##N = underlying::SQ_C##N; \
    static constexpr auto SQ_D##N = underlying::SQ_D##N; \
    static constexpr auto SQ_E##N = underlying::SQ_E##N; \
    static constexpr auto SQ_F##N = underlying::SQ_F##N; \
    static constexpr auto SQ_G##N = underlying::SQ_G##N; \
    static constexpr auto SQ_H##N = underlying::SQ_H##N;

class File {
   public:
    enum class underlying : std::uint8_t { FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H, NO_FILE };

    constexpr File() : file(underlying::NO_FILE) {}
    constexpr File(underlying file) : file(file) {}
    constexpr File(int file) : file(static_cast<underlying>(file)) {}
    constexpr File(std::string_view sw)
        : file(static_cast<underlying>(static_cast<char>(utils::tolower(static_cast<unsigned char>(sw[0]))) - 'a')) {}

    [[nodiscard]] constexpr underlying internal() const noexcept { return file; }

    constexpr bool operator==(const File& rhs) const noexcept { return file == rhs.file; }
    constexpr bool operator!=(const File& rhs) const noexcept { return file != rhs.file; }

    constexpr bool operator==(const underlying& rhs) const noexcept { return file == rhs; }
    constexpr bool operator!=(const underlying& rhs) const noexcept { return file != rhs; }

    constexpr bool operator>=(const File& rhs) const noexcept {
        return static_cast<int>(file) >= static_cast<int>(rhs.file);
    }
    constexpr bool operator<=(const File& rhs) const noexcept {
        return static_cast<int>(file) <= static_cast<int>(rhs.file);
    }

    constexpr bool operator>(const File& rhs) const noexcept {
        return static_cast<int>(file) > static_cast<int>(rhs.file);
    }

    constexpr bool operator<(const File& rhs) const noexcept {
        return static_cast<int>(file) < static_cast<int>(rhs.file);
    }

    constexpr operator int() const noexcept { return static_cast<int>(file); }

    explicit operator std::string() const { return std::string(1, static_cast<char>(static_cast<int>(file) + 'a')); }

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
    enum class underlying { RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, NO_RANK };

    constexpr Rank() : rank_(underlying::NO_RANK) {}
    constexpr Rank(underlying rank) : rank_(rank) {}
    constexpr Rank(int rank) : rank_(static_cast<underlying>(rank)) {}
    constexpr Rank(std::string_view sw)
        : rank_(static_cast<underlying>(static_cast<char>(utils::tolower(static_cast<unsigned char>(sw[0]))) - '1')) {}

    [[nodiscard]] constexpr underlying internal() const noexcept { return rank_; }

    constexpr bool operator==(const Rank& rhs) const noexcept { return rank_ == rhs.rank_; }
    constexpr bool operator!=(const Rank& rhs) const noexcept { return rank_ != rhs.rank_; }

    constexpr bool operator==(const underlying& rhs) const noexcept { return rank_ == rhs; }
    constexpr bool operator!=(const underlying& rhs) const noexcept { return rank_ != rhs; }

    constexpr bool operator>=(const Rank& rhs) const noexcept {
        return static_cast<int>(rank_) >= static_cast<int>(rhs.rank_);
    }
    constexpr bool operator<=(const Rank& rhs) const noexcept {
        return static_cast<int>(rank_) <= static_cast<int>(rhs.rank_);
    }

    operator std::string() const { return std::string(1, static_cast<char>(static_cast<int>(rank_) + '1')); }

    constexpr operator int() const noexcept { return static_cast<int>(rank_); }

    constexpr std::uint64_t bb() const noexcept { return 0xffULL << (8 * static_cast<int>(rank_)); }

    [[nodiscard]] static constexpr bool back_rank(Rank r, Color color) noexcept {
        if (color == Color::WHITE) return r == Rank::RANK_1;
        return r == Rank::RANK_8;
    }

    [[nodiscard]] static constexpr Rank rank(Rank r, Color color) noexcept {
        if (color == Color::WHITE) return r;
        return Rank(static_cast<underlying>(static_cast<int>(RANK_8) - static_cast<int>(r)));
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
    underlying rank_;
};

class Square {
   public:
    // clang-format off
    enum class underlying {
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

// when c++20
#if __cplusplus >= 202002L
    using enum underlying;
#else

    CHESS_DECLARE_RANK(1)
    CHESS_DECLARE_RANK(2)
    CHESS_DECLARE_RANK(3)
    CHESS_DECLARE_RANK(4)
    CHESS_DECLARE_RANK(5)
    CHESS_DECLARE_RANK(6)
    CHESS_DECLARE_RANK(7)
    CHESS_DECLARE_RANK(8)

    static constexpr auto NO_SQ = underlying::NO_SQ;

#endif

    constexpr Square() : sq(underlying::NO_SQ) {}

    constexpr Square(int sq) : sq(static_cast<underlying>(sq)) { assert(sq <= 64 && sq >= 0); }
    constexpr Square(File file, Rank rank) : sq(static_cast<underlying>(file + rank * 8)) {}
    constexpr Square(Rank rank, File file) : sq(static_cast<underlying>(file + rank * 8)) {}
    constexpr Square(underlying sq) : sq(sq) {}
    constexpr Square(std::string_view str) : sq(static_cast<underlying>((str[0] - 'a') + (str[1] - '1') * 8)) {
        assert(str.size() >= 2);
    }

    constexpr Square operator^(const Square& s) const noexcept {
        return Square(static_cast<underlying>(static_cast<int>(sq) ^ s.index()));
    };

    constexpr bool operator==(const Square& rhs) const noexcept { return sq == rhs.sq; }

    constexpr bool operator!=(const Square& rhs) const noexcept { return sq != rhs.sq; }

    constexpr bool operator>(const Square& rhs) const noexcept {
        return static_cast<int>(sq) > static_cast<int>(rhs.sq);
    }

    constexpr bool operator>=(const Square& rhs) const noexcept {
        return static_cast<int>(sq) >= static_cast<int>(rhs.sq);
    }

    constexpr bool operator<(const Square& rhs) const noexcept {
        return static_cast<int>(sq) < static_cast<int>(rhs.sq);
    }

    constexpr bool operator<=(const Square& rhs) const noexcept {
        return static_cast<int>(sq) <= static_cast<int>(rhs.sq);
    }

    constexpr Square operator+(const Square& rhs) const noexcept {
        return Square(static_cast<underlying>(static_cast<int>(sq) + static_cast<int>(rhs.sq)));
    }

    constexpr Square operator-(const Square& rhs) const noexcept {
        return Square(static_cast<underlying>(static_cast<int>(sq) - static_cast<int>(rhs.sq)));
    }

    constexpr Square& operator++() noexcept {
        sq = static_cast<underlying>(static_cast<int>(sq) + 1);
        return *this;
    }

    constexpr Square operator++(int) noexcept {
        Square tmp(*this);
        operator++();
        return tmp;
    }

    constexpr Square& operator--() noexcept {
        sq = static_cast<underlying>(static_cast<int>(sq) - 1);
        return *this;
    }

    constexpr Square operator--(int) noexcept {
        Square tmp(*this);
        operator--();
        return tmp;
    }

    /**
     * @brief Get a string representation of the square.
     */
    [[nodiscard]] operator std::string() const {
        std::string str;
        str += static_cast<std::string>(file());
        str += static_cast<std::string>(rank());
        return str;
    }

    [[nodiscard]] constexpr int index() const noexcept { return static_cast<int>(sq); }

    [[nodiscard]] constexpr File file() const noexcept { return File(index() & 7); }
    [[nodiscard]] constexpr Rank rank() const noexcept { return Rank(index() >> 3); }

    /**
     * @brief Check if the square is light.
     * @return
     */
    [[nodiscard]] constexpr bool is_light() const noexcept {
        return (static_cast<std::int8_t>(sq) / 8 + static_cast<std::int8_t>(sq) % 8) % 2 == 0;
    }

    /**
     * @brief Check if the square is dark.
     * @return
     */
    [[nodiscard]] constexpr bool is_dark() const noexcept { return !is_light(); }

    /**
     * @brief Check if the square is vali.d
     * @return
     */
    [[nodiscard]] constexpr bool is_valid() const noexcept { return static_cast<std::int8_t>(sq) < 64; }

    /**
     * @brief Check if the square is valid.
     * @param r
     * @param f
     * @return
     */
    [[nodiscard]] constexpr static bool is_valid(Rank r, File f) noexcept {
        return r >= Rank::RANK_1 && r <= Rank::RANK_8 && f >= File::FILE_A && f <= File::FILE_H;
    }

    /**
     * @brief Get the chebyshev distance between two squares.
     * @param sq
     * @param sq2
     * @return
     */
    [[nodiscard]] static int distance(Square sq, Square sq2) noexcept {
        return std::max(std::abs(sq.file() - sq2.file()), std::abs(sq.rank() - sq2.rank()));
    }

    /**
     * @brief Absolute value of sq - sq2.
     * @param sq
     * @param sq2
     * @return
     */
    [[nodiscard]] static int value_distance(Square sq, Square sq2) noexcept {
        return std::abs(sq.index() - sq2.index());
    }

    /**
     * @brief Check if the squares share the same color. I.e. if they are both light or dark.
     * @param sq
     * @param sq2
     * @return
     */
    [[nodiscard]] static constexpr bool same_color(Square sq, Square sq2) noexcept {
        return ((9 * (sq ^ sq2).index()) & 8) == 0;
    }

    /**
     * @brief Check if the square is on the back rank.
     * @param sq
     * @param color
     * @return
     */
    [[nodiscard]] static constexpr bool back_rank(Square sq, Color color) noexcept {
        if (color == Color::WHITE)
            return sq.rank() == Rank::RANK_1;
        else
            return sq.rank() == Rank::RANK_8;
    }

    /**
     * @brief Flips the square vertically.
     * @return
     */
    constexpr Square& flip() noexcept {
        sq = static_cast<underlying>(static_cast<int>(sq) ^ 56);
        return *this;
    }

    /**
     * @brief Flips the square vertically, depending on the color.
     * @param c
     * @return
     */
    [[nodiscard]] constexpr Square relative_square(Color c) const noexcept {
        return Square(static_cast<int>(sq) ^ (c * 56));
    }

    [[nodiscard]] constexpr int diagonal_of() const noexcept { return 7 + rank() - file(); }

    [[nodiscard]] constexpr int antidiagonal_of() const noexcept { return rank() + file(); }

    /**
     * @brief Get the en passant square. Should only be called for valid ep positions.
     * @return
     */
    [[nodiscard]] constexpr Square ep_square() const noexcept {
        assert(rank() == Rank::RANK_3     // capture
               || rank() == Rank::RANK_4  // push
               || rank() == Rank::RANK_5  // push
               || rank() == Rank::RANK_6  // capture
        );
        return Square(static_cast<int>(sq) ^ 8);
    }

    /**
     * @brief Get the destination square of the king after castling.
     * @param is_king_side
     * @param c
     * @return
     */
    [[nodiscard]] static constexpr Square castling_king_square(bool is_king_side, Color c) noexcept {
        return Square(is_king_side ? Square::SQ_G1 : Square::SQ_C1).relative_square(c);
    }

    /**
     * @brief Get the destination square of the rook after castling.
     * @param is_king_side
     * @param c
     * @return
     */
    [[nodiscard]] static constexpr Square castling_rook_square(bool is_king_side, Color c) noexcept {
        return Square(is_king_side ? Square::SQ_F1 : Square::SQ_D1).relative_square(c);
    }

    /**
     * @brief Maximum number of squares.
     * @return
     */
    [[nodiscard]] static constexpr int max() noexcept { return 64; }

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

[[nodiscard]] constexpr Direction make_direction(Direction dir, Color c) noexcept {
    if (c == Color::BLACK) return static_cast<Direction>(-static_cast<int8_t>(dir));
    return dir;
}

constexpr Square operator+(Square sq, Direction dir) {
    return static_cast<Square>(sq.index() + static_cast<int8_t>(dir));
}

#undef CHESS_DECLARE_RANK

}  // namespace chess
