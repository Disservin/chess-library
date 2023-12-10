#pragma once

#include <cstdint>
#include <cassert>

#include "color.hpp"

namespace chess {

class PieceType {
   public:
    enum class underlying : std::uint8_t { PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING, NONE };

    constexpr PieceType() : pt(underlying::NONE) {}
    constexpr PieceType(underlying pt) : pt(pt) {}
    constexpr PieceType(char type) {
        switch (type) {
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
            default:
                pt = underlying::NONE;
                break;
        }
    }

    constexpr explicit operator char() const {
        switch (pt) {
            case underlying::PAWN:
                return 'p';
            case underlying::KNIGHT:
                return 'n';
            case underlying::BISHOP:
                return 'b';
            case underlying::ROOK:
                return 'r';
            case underlying::QUEEN:
                return 'q';
            case underlying::KING:
                return 'k';
            default:
                return ' ';
        }
    }

    constexpr bool operator==(const PieceType& rhs) const { return pt == rhs.pt; }
    constexpr bool operator!=(const PieceType& rhs) const { return pt != rhs.pt; }

    constexpr underlying internal() const { return pt; }

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
    os << static_cast<char>(pt);
    return os;
}

class Piece {
   public:
    enum class underlying : std::uint8_t {
        NONE,
        WHITE_PAWN   = 1,
        WHITE_KNIGHT = 2,
        WHITE_BISHOP = 3,
        WHITE_ROOK   = 4,
        WHITE_QUEEN  = 5,
        WHITE_KING   = 6,
        BLACK_PAWN   = 9,
        BLACK_KNIGHT = 10,
        BLACK_BISHOP = 11,
        BLACK_ROOK   = 12,
        BLACK_QUEEN  = 13,
        BLACK_KING   = 14
    };

    constexpr Piece() : piece(underlying::NONE) {}
    constexpr Piece(underlying piece) : piece(piece) {}
    constexpr Piece(PieceType type, Color color)
        : piece(static_cast<underlying>(static_cast<std::uint8_t>(type.internal()) |
                                        (static_cast<std::uint8_t>(color.internal()) << 3))) {}
    constexpr Piece(char p) {
        switch (p) {
            case 'P':
                piece = WHITE_PAWN;
                break;
            case 'N':
                piece = WHITE_KNIGHT;
                break;
            case 'B':
                piece = WHITE_BISHOP;
                break;
            case 'R':
                piece = WHITE_ROOK;
                break;
            case 'Q':
                piece = WHITE_QUEEN;
                break;
            case 'K':
                piece = WHITE_KING;
                break;
            // black
            case 'p':
                piece = BLACK_PAWN;
                break;
            case 'n':
                piece = BLACK_KNIGHT;
                break;
            case 'b':
                piece = BLACK_BISHOP;
                break;
            case 'r':
                piece = BLACK_ROOK;
                break;
            case 'q':
                piece = BLACK_QUEEN;
                break;
            case 'k':
                piece = BLACK_KING;
                break;
            default:
                piece = NONE;
                break;
        }
    }

    constexpr PieceType type() const { return static_cast<PieceType>(int(piece) & 7); }
    constexpr Color color() const { return static_cast<Color>(int(piece) >> 3); }

    constexpr underlying internal() const { return piece; }

    constexpr bool operator==(const Piece& rhs) const { return piece == rhs.piece; }
    constexpr bool operator!=(const Piece& rhs) const { return piece != rhs.piece; }

    constexpr bool operator==(const underlying& rhs) const { return piece == rhs; }
    constexpr bool operator!=(const underlying& rhs) const { return piece != rhs; }

    constexpr bool operator==(const PieceType& rhs) const { return type() == rhs; }
    constexpr bool operator!=(const PieceType& rhs) const { return type() != rhs; }

    constexpr bool operator<(const Piece& rhs) const { return piece < rhs.piece; }

    explicit operator char() const {
        switch (piece) {
            case WHITE_PAWN:
                return 'P';
            case WHITE_KNIGHT:
                return 'N';
            case WHITE_BISHOP:
                return 'B';
            case WHITE_ROOK:
                return 'R';
            case WHITE_QUEEN:
                return 'Q';
            case WHITE_KING:
                return 'K';
            // black
            case BLACK_PAWN:
                return 'p';
            case BLACK_KNIGHT:
                return 'n';
            case BLACK_BISHOP:
                return 'b';
            case BLACK_ROOK:
                return 'r';
            case BLACK_QUEEN:
                return 'q';
            case BLACK_KING:
                return 'k';
            default:
                assert(false);
                return ' ';
        }
    }

    static constexpr underlying NONE         = underlying::NONE;
    static constexpr underlying WHITE_PAWN   = underlying::WHITE_PAWN;
    static constexpr underlying WHITE_KNIGHT = underlying::WHITE_KNIGHT;
    static constexpr underlying WHITE_BISHOP = underlying::WHITE_BISHOP;
    static constexpr underlying WHITE_ROOK   = underlying::WHITE_ROOK;
    static constexpr underlying WHITE_QUEEN  = underlying::WHITE_QUEEN;
    static constexpr underlying WHITE_KING   = underlying::WHITE_KING;
    static constexpr underlying BLACK_PAWN   = underlying::BLACK_PAWN;
    static constexpr underlying BLACK_KNIGHT = underlying::BLACK_KNIGHT;
    static constexpr underlying BLACK_BISHOP = underlying::BLACK_BISHOP;
    static constexpr underlying BLACK_ROOK   = underlying::BLACK_ROOK;
    static constexpr underlying BLACK_QUEEN  = underlying::BLACK_QUEEN;
    static constexpr underlying BLACK_KING   = underlying::BLACK_KING;

   private:
    underlying piece;
};
}  // namespace chess
