#pragma once

#include <cstdint>
#include <cassert>

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
        switch (type.data()[0]) {
            case 'P':
                pt = underlying::PAWN;
                break;
            case 'N':
                pt = underlying::KNIGHT;
                break;
            case 'B':
                pt = underlying::BISHOP;
                break;
            case 'R':
                pt = underlying::ROOK;
                break;
            case 'Q':
                pt = underlying::QUEEN;
                break;
            case 'K':
                pt = underlying::KING;
                break;

            case 'p':
                pt = underlying::PAWN;
                break;
            case 'n':
                pt = underlying::KNIGHT;
                break;
            case 'b':
                pt = underlying::BISHOP;
                break;
            case 'r':
                pt = underlying::ROOK;
                break;
            case 'q':
                pt = underlying::QUEEN;
                break;
            case 'k':
                pt = underlying::KING;
                break;
            default:
                pt = underlying::NONE;
                break;
        }
    }

    explicit operator std::string() const {
        switch (pt) {
            case underlying::PAWN:
                return "p";
            case underlying::KNIGHT:
                return "n";
            case underlying::BISHOP:
                return "b";
            case underlying::ROOK:
                return "r";
            case underlying::QUEEN:
                return "q";
            case underlying::KING:
                return "k";
            default:
                return " ";
        }
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
    constexpr Piece(std::string_view p) : piece(underlying::NONE) {
        switch (p.data()[0]) {
            case 'P':
                piece = WHITEPAWN;
                break;
            case 'N':
                piece = WHITEKNIGHT;
                break;
            case 'B':
                piece = WHITEBISHOP;
                break;
            case 'R':
                piece = WHITEROOK;
                break;
            case 'Q':
                piece = WHITEQUEEN;
                break;
            case 'K':
                piece = WHITEKING;
                break;
            // black
            case 'p':
                piece = BLACKPAWN;
                break;
            case 'n':
                piece = BLACKKNIGHT;
                break;
            case 'b':
                piece = BLACKBISHOP;
                break;
            case 'r':
                piece = BLACKROOK;
                break;
            case 'q':
                piece = BLACKQUEEN;
                break;
            case 'k':
                piece = BLACKKING;
                break;
            default:
                piece = NONE;
                break;
        }
    }

    constexpr bool operator<(const Piece& rhs) const noexcept { return piece < rhs.piece; }
    constexpr bool operator>(const Piece& rhs) const noexcept { return piece > rhs.piece; }
    constexpr bool operator==(const Piece& rhs) const noexcept { return piece == rhs.piece; }
    constexpr bool operator!=(const Piece& rhs) const noexcept { return piece != rhs.piece; }

    constexpr bool operator==(const underlying& rhs) const noexcept { return piece == rhs; }
    constexpr bool operator!=(const underlying& rhs) const noexcept { return piece != rhs; }

    constexpr bool operator==(const PieceType& rhs) const noexcept { return type() == rhs; }
    constexpr bool operator!=(const PieceType& rhs) const noexcept { return type() != rhs; }

    explicit operator std::string() const {
        switch (piece) {
            case WHITEPAWN:
                return "P";
            case WHITEKNIGHT:
                return "N";
            case WHITEBISHOP:
                return "B";
            case WHITEROOK:
                return "R";
            case WHITEQUEEN:
                return "Q";
            case WHITEKING:
                return "K";
            // black
            case BLACKPAWN:
                return "p";
            case BLACKKNIGHT:
                return "n";
            case BLACKBISHOP:
                return "b";
            case BLACKROOK:
                return "r";
            case BLACKQUEEN:
                return "q";
            case BLACKKING:
                return "k";
            default:
                return ".";
        }
    }

    operator int() const noexcept { return static_cast<int>(piece); }

    [[nodiscard]] constexpr PieceType type() const noexcept {
        return static_cast<PieceType::underlying>(int(piece) % 6);
    }

    [[nodiscard]] constexpr Color color() const noexcept {
        if (piece == NONE) {
            return Color::NONE;
        }
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
};
}  // namespace chess
