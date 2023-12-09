#pragma once

#include <cstdint>
#include <cassert>

#include "color.hpp"

namespace chess {

enum class PieceType : std::uint8_t { PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING, NO_PIECE_TYPE };

enum ColoredPiece : std::uint8_t {
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

class Piece {
   public:
    constexpr Piece() = default;
    constexpr Piece(ColoredPiece piece) : piece(piece) {}
    constexpr Piece(PieceType type, Color color)
        : piece(static_cast<ColoredPiece>(static_cast<std::uint8_t>(type) |
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
                assert(false);
                break;
        }
    }

    constexpr PieceType type() const { return static_cast<PieceType>(piece & 7); }
    constexpr Color color() const { return static_cast<Color>(piece >> 3); }

    constexpr bool operator==(const Piece& rhs) const { return piece == rhs.piece; }
    constexpr bool operator!=(const Piece& rhs) const { return piece != rhs.piece; }

    constexpr bool operator==(const ColoredPiece& rhs) const { return piece == rhs; }
    constexpr bool operator!=(const ColoredPiece& rhs) const { return piece != rhs; }

    constexpr bool operator==(const PieceType& rhs) const { return type() == rhs; }
    constexpr bool operator!=(const PieceType& rhs) const { return type() != rhs; }

    operator char() const {
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

   private:
    ColoredPiece piece;
};
}  // namespace chess
