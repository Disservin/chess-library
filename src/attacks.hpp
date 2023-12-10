#pragma once

#include <cstdint>
#include <cassert>
#include <functional>

#include "piece.hpp"
#include "coords.hpp"
#include "bitboard.hpp"
#include "attacks_fwd.hpp"
#include "board.hpp"

namespace chess {

/// @brief Shifts a bitboard in a given direction
/// @tparam direction
/// @param b
/// @return
template <Direction direction>
[[nodiscard]] inline constexpr Bitboard attacks::shift(const Bitboard b) {
    switch (direction) {
        case Direction::NORTH:
            return b << 8;
        case Direction::SOUTH:
            return b >> 8;
        case Direction::NORTH_WEST:
            return (b & ~MASK_FILE[0]) << 7;
        case Direction::WEST:
            return (b & ~MASK_FILE[0]) >> 1;
        case Direction::SOUTH_WEST:
            return (b & ~MASK_FILE[0]) >> 9;
        case Direction::NORTH_EAST:
            return (b & ~MASK_FILE[7]) << 9;
        case Direction::EAST:
            return (b & ~MASK_FILE[7]) << 1;
        case Direction::SOUTH_EAST:
            return (b & ~MASK_FILE[7]) >> 7;
    }
}

/// @brief [Internal Usage] Generate the left side pawn attacks.
/// @tparam c
/// @param pawns
/// @return
template <Color::underlying c>
[[nodiscard]] inline Bitboard attacks::pawnLeftAttacks(const Bitboard pawns) {
    return c == Color::WHITE ? (pawns << 7) & ~MASK_FILE[static_cast<int>(File::FILE_H)]
                             : (pawns >> 7) & ~MASK_FILE[static_cast<int>(File::FILE_A)];
}

/// @brief [Internal Usage] Generate the right side pawn attacks.
/// @tparam c
/// @param pawns
/// @return
template <Color::underlying c>
[[nodiscard]] inline Bitboard attacks::pawnRightAttacks(const Bitboard pawns) {
    return c == Color::WHITE ? (pawns << 9) & ~MASK_FILE[static_cast<int>(File::FILE_A)]
                             : (pawns >> 9) & ~MASK_FILE[static_cast<int>(File::FILE_H)];
}

/// @brief Returns the pawn attacks for a given color and square
/// @param c
/// @param sq
/// @return
[[nodiscard]] inline Bitboard attacks::pawn(Color c, Square sq) {
    return PawnAttacks[int(c.internal())][static_cast<int>(sq.internal())];
}

/// @brief Returns the knight attacks for a given square
/// @param sq
/// @return
[[nodiscard]] inline Bitboard attacks::knight(Square sq) {
    return KnightAttacks[static_cast<int>(sq.internal())];
}

/// @brief Returns the bishop attacks for a given square
/// @param sq
/// @param occupied
/// @return
[[nodiscard]] inline Bitboard attacks::bishop(Square sq, Bitboard occupied) {
    return BishopTable[static_cast<int>(sq.internal())]
        .attacks[BishopTable[static_cast<int>(sq.internal())](occupied)];
}

/// @brief Returns the rook attacks for a given square
/// @param sq
/// @param occupied
/// @return
[[nodiscard]] inline Bitboard attacks::rook(Square sq, Bitboard occupied) {
    return RookTable[static_cast<int>(sq.internal())]
        .attacks[RookTable[static_cast<int>(sq.internal())](occupied)];
}

/// @brief Returns the queen attacks for a given square
/// @param sq
/// @param occupied
/// @return
[[nodiscard]] inline Bitboard attacks::queen(Square sq, Bitboard occupied) {
    return bishop(sq, occupied) | rook(sq, occupied);
}

/// @brief Returns the king attacks for a given square
/// @param sq
/// @return
[[nodiscard]] inline Bitboard attacks::king(Square sq) {
    return KingAttacks[static_cast<int>(sq.internal())];
}

/// @brief Returns a bitboard with the origin squares of the attacking pieces set
/// @param board
/// @param color Attacker Color
/// @param square Attacked Square
/// @param occupied
/// @return
[[nodiscard]] inline Bitboard attacks::attackers(const Board &board, Color color, Square square,
                                                 Bitboard occupied) {
    const auto queens = board.pieces(PieceType::QUEEN, color);

    // using the fact that if we can attack PieceType from square, they can attack us back
    auto atks = (pawn(~color, square) & board.pieces(PieceType::PAWN, color));
    atks |= (knight(square) & board.pieces(PieceType::KNIGHT, color));
    atks |= (bishop(square, occupied) & (board.pieces(PieceType::BISHOP, color) | queens));
    atks |= (rook(square, occupied) & (board.pieces(PieceType::ROOK, color) | queens));
    atks |= (king(square) & board.pieces(PieceType::KING, color));

    return atks & occupied;
}

/// @brief [Internal Usage] Slow function to calculate bishop attacks
/// @param sq
/// @param occupied
/// @return
[[nodiscard]] inline Bitboard attacks::bishopAttacks(Square sq, Bitboard occupied) {
    Bitboard attacks = 0ULL;

    int r, f;

    int br = static_cast<int>(sq.rank().internal());
    int bf = static_cast<int>(sq.file().internal());

    for (r = br + 1, f = bf + 1; utils::validSq(static_cast<Rank>(r), static_cast<File>(f));
         r++, f++) {
        auto s = static_cast<int>(Square(static_cast<Rank>(r), static_cast<File>(f)).internal());
        attacks |= (1ULL << s);
        if (occupied.check(s)) break;
    }

    for (r = br - 1, f = bf + 1; utils::validSq(static_cast<Rank>(r), static_cast<File>(f));
         r--, f++) {
        auto s = static_cast<int>(Square(static_cast<Rank>(r), static_cast<File>(f)).internal());
        attacks |= (1ULL << s);
        if (occupied.check(s)) break;
    }

    for (r = br + 1, f = bf - 1; utils::validSq(static_cast<Rank>(r), static_cast<File>(f));
         r++, f--) {
        auto s = static_cast<int>(Square(static_cast<Rank>(r), static_cast<File>(f)).internal());
        attacks |= (1ULL << s);
        if (occupied.check(s)) break;
    }

    for (r = br - 1, f = bf - 1; utils::validSq(static_cast<Rank>(r), static_cast<File>(f));
         r--, f--) {
        auto s = static_cast<int>(Square(static_cast<Rank>(r), static_cast<File>(f)).internal());
        attacks |= (1ULL << s);
        if (occupied.check(s)) break;
    }

    return attacks;
}

/// @brief [Internal Usage] Slow function to calculate rook attacks
/// @param sq
/// @param occupied
/// @return
[[nodiscard]] inline Bitboard attacks::rookAttacks(Square sq, Bitboard occupied) {
    Bitboard attacks = 0ULL;

    int r, f;

    int rr = static_cast<int>(sq.rank().internal());
    int rf = static_cast<int>(sq.file().internal());

    for (r = rr + 1; utils::validSq(static_cast<Rank>(r), static_cast<File>(rf)); r++) {
        auto s = static_cast<int>(Square(static_cast<Rank>(r), static_cast<File>(rf)).internal());
        attacks |= (1ULL << s);
        if (occupied.check(s)) break;
    }

    for (r = rr - 1; utils::validSq(static_cast<Rank>(r), static_cast<File>(rf)); r--) {
        auto s = static_cast<int>(Square(static_cast<Rank>(r), static_cast<File>(rf)).internal());
        attacks |= (1ULL << s);
        if (occupied.check(s)) break;
    }

    for (f = rf + 1; utils::validSq(static_cast<Rank>(rr), static_cast<File>(f)); f++) {
        auto s = static_cast<int>(Square(static_cast<Rank>(rr), static_cast<File>(f)).internal());
        attacks |= (1ULL << s);
        if (occupied.check(s)) break;
    }

    for (f = rf - 1; utils::validSq(static_cast<Rank>(rr), static_cast<File>(f)); f--) {
        auto s = static_cast<int>(Square(static_cast<Rank>(rr), static_cast<File>(f)).internal());
        attacks |= (1ULL << s);
        if (occupied.check(s)) break;
    }

    return attacks;
}

/// @brief [Internal Usage] Initializes the magic bitboard tables for sliding pieces
/// @param sq
/// @param table
/// @param magic
/// @param attacks
inline void attacks::initSliders(Square sq, Magic table[], U64 magic,
                                 const std::function<Bitboard(Square, Bitboard)> &attacks) {
    const Bitboard edges =
        ((MASK_RANK[static_cast<int>(Rank::RANK_1)] | MASK_RANK[static_cast<int>(Rank::RANK_8)]) &
         ~MASK_RANK[static_cast<int>(sq.rank().internal())]) |
        ((MASK_FILE[static_cast<int>(File::FILE_A)] | MASK_FILE[static_cast<int>(File::FILE_H)]) &
         ~MASK_FILE[static_cast<int>(sq.file().internal())]);

    Bitboard occ   = 0ULL;
    const auto sq_ = static_cast<int>(sq.internal());

    table[sq_].magic = magic;
    table[sq_].mask  = attacks(sq, occ) & ~edges;
    table[sq_].shift = 64 - table[sq_].mask.count();

    if (sq < 64 - 1) {
        table[sq_ + 1].attacks = table[sq_].attacks + (1 << table[sq_].mask.count());
    }

    do {
        table[sq_].attacks[table[sq_](occ)] = attacks(sq, occ);
        occ                                 = (occ - table[sq_].mask) & table[sq_].mask;
    } while (occ);
}

/// @brief [Internal Usage] Initializes the attacks for the bishop and rook. Called once at
/// startup.
inline void attacks::initAttacks() {
    BishopTable[0].attacks = BishopAttacks;
    RookTable[0].attacks   = RookAttacks;

    for (int i = 0; i < 64; i++) {
        initSliders(static_cast<Square>(i), BishopTable, BishopMagics[i], bishopAttacks);
        initSliders(static_cast<Square>(i), RookTable, RookMagics[i], rookAttacks);
    }
}

static auto init = []() {
    attacks::initAttacks();
    return 0;
}();
}  // namespace chess
