#pragma once

#include <cstdint>

#include "color.hpp"

namespace chess {

class PieceType {
   public:
    enum class underlying : std::uint8_t {
        PAWN,
        KNIGHT,
        BISHOP,
        ROOK,
        QUEEN,
        KING,
        NONE,
    };

    constexpr PieceType() : pt(underlying::NONE) {}
    constexpr PieceType(underlying pt) : pt(pt) {}
    constexpr explicit PieceType(std::string_view type) : pt(underlying::NONE) {
        assert(type.size() > 0);

        char c = type[0];

        if (c == 'P' || c == 'p')
            pt = underlying::PAWN;
        else if (c == 'N' || c == 'n')
            pt = underlying::KNIGHT;
        else if (c == 'B' || c == 'b')
            pt = underlying::BISHOP;
        else if (c == 'R' || c == 'r')
            pt = underlying::ROOK;
        else if (c == 'Q' || c == 'q')
            pt = underlying::QUEEN;
        else if (c == 'K' || c == 'k')
            pt = underlying::KING;
        else
            pt = underlying::NONE;
    }

    explicit operator std::string() const {
        if (pt == underlying::NONE) return " ";
        constexpr static const char* pieceTypeStr[] = {"p", "n", "b", "r", "q", "k"};
        return pieceTypeStr[static_cast<int>(pt)];
    }

    constexpr bool operator==(const PieceType& rhs) const noexcept { return pt == rhs.pt; }
    constexpr bool operator!=(const PieceType& rhs) const noexcept { return pt != rhs.pt; }

    constexpr operator int() const noexcept { return static_cast<int>(pt); }

    [[nodiscard]] constexpr underlying internal() const noexcept { return pt; }

    static constexpr underlying PAWN   = underlying::PAWN;
    static constexpr underlying KNIGHT = underlying::KNIGHT;
    static constexpr underlying BISHOP = underlying::BISHOP;
    static constexpr underlying ROOK   = underlying::ROOK;
    static constexpr underlying QUEEN  = underlying::QUEEN;
    static constexpr underlying KING   = underlying::KING;
    static constexpr underlying NONE   = underlying::NONE;

   private:
    underlying pt;
};

inline std::ostream& operator<<(std::ostream& os, const PieceType& pt) {
    os << static_cast<std::string>(pt);
    return os;
}

class Piece {
   public:
    enum class underlying : std::uint8_t {
        WHITEPAWN,
        WHITEKNIGHT,
        WHITEBISHOP,
        WHITEROOK,
        WHITEQUEEN,
        WHITEKING,
        BLACKPAWN,
        BLACKKNIGHT,
        BLACKBISHOP,
        BLACKROOK,
        BLACKQUEEN,
        BLACKKING,
        NONE
    };

    constexpr Piece() : piece(underlying::NONE) {}
    constexpr Piece(underlying piece) : piece(piece) {}
    constexpr Piece(PieceType type, Color color)
        : piece(color == Color::NONE      ? Piece::NONE
                : type == PieceType::NONE ? Piece::NONE
                                          : static_cast<underlying>(static_cast<int>(color.internal()) * 6 + type)) {}
    constexpr Piece(Color color, PieceType type)
        : piece(color == Color::NONE      ? Piece::NONE
                : type == PieceType::NONE ? Piece::NONE
                                          : static_cast<underlying>(static_cast<int>(color.internal()) * 6 + type)) {}
    constexpr Piece(std::string_view p) : piece(underlying::NONE) { piece = convertCharToUnderlying(p[0]); }

    constexpr bool operator<(const Piece& rhs) const noexcept { return piece < rhs.piece; }
    constexpr bool operator>(const Piece& rhs) const noexcept { return piece > rhs.piece; }
    constexpr bool operator==(const Piece& rhs) const noexcept { return piece == rhs.piece; }
    constexpr bool operator!=(const Piece& rhs) const noexcept { return piece != rhs.piece; }

    constexpr bool operator==(const underlying& rhs) const noexcept { return piece == rhs; }
    constexpr bool operator!=(const underlying& rhs) const noexcept { return piece != rhs; }

    constexpr bool operator==(const PieceType& rhs) const noexcept { return type() == rhs; }
    constexpr bool operator!=(const PieceType& rhs) const noexcept { return type() != rhs; }

    explicit operator std::string() const {
        constexpr static const char* pieceStr[] = {"P", "N", "B", "R", "Q", "K",  //
                                                   "p", "n", "b", "r", "q", "k"};
        if (piece == NONE) return ".";
        return pieceStr[static_cast<int>(piece)];
    }

    explicit operator std::string_view() const {
        constexpr static const char* pieceStr[] = {"P", "N", "B", "R", "Q", "K",  //
                                                   "p", "n", "b", "r", "q", "k"};
        if (piece == NONE) return ".";
        return std::string_view(pieceStr[static_cast<int>(piece)], 1);
    }

    constexpr operator int() const noexcept { return static_cast<int>(piece); }

    [[nodiscard]] constexpr PieceType type() const noexcept {
        if (piece == NONE) return PieceType::NONE;
        // return static_cast<PieceType::underlying>(int(piece) % 6);
        return static_cast<PieceType::underlying>(static_cast<int>(piece) > 5 ? static_cast<int>(piece) - 6
                                                                              : static_cast<int>(piece));
    }

    [[nodiscard]] constexpr Color color() const noexcept {
        if (piece == NONE) return Color::NONE;
        return static_cast<Color>(static_cast<int>(piece) / 6);
    }

    [[nodiscard]] constexpr underlying internal() const noexcept { return piece; }

    static constexpr underlying NONE        = underlying::NONE;
    static constexpr underlying WHITEPAWN   = underlying::WHITEPAWN;
    static constexpr underlying WHITEKNIGHT = underlying::WHITEKNIGHT;
    static constexpr underlying WHITEBISHOP = underlying::WHITEBISHOP;
    static constexpr underlying WHITEROOK   = underlying::WHITEROOK;
    static constexpr underlying WHITEQUEEN  = underlying::WHITEQUEEN;
    static constexpr underlying WHITEKING   = underlying::WHITEKING;
    static constexpr underlying BLACKPAWN   = underlying::BLACKPAWN;
    static constexpr underlying BLACKKNIGHT = underlying::BLACKKNIGHT;
    static constexpr underlying BLACKBISHOP = underlying::BLACKBISHOP;
    static constexpr underlying BLACKROOK   = underlying::BLACKROOK;
    static constexpr underlying BLACKQUEEN  = underlying::BLACKQUEEN;
    static constexpr underlying BLACKKING   = underlying::BLACKKING;

   private:
    underlying piece;

    [[nodiscard]] constexpr static underlying convertCharToUnderlying(char c) {
        switch (c) {
            case 'P':
                return WHITEPAWN;
            case 'N':
                return WHITEKNIGHT;
            case 'B':
                return WHITEBISHOP;
            case 'R':
                return WHITEROOK;
            case 'Q':
                return WHITEQUEEN;
            case 'K':
                return WHITEKING;
            case 'p':
                return BLACKPAWN;
            case 'n':
                return BLACKKNIGHT;
            case 'b':
                return BLACKBISHOP;
            case 'r':
                return BLACKROOK;
            case 'q':
                return BLACKQUEEN;
            case 'k':
                return BLACKKING;
            default:
                return NONE;
        }
    }
};
}  // namespace chess
