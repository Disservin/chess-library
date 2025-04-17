#pragma once

#include <functional>
#include <utility>

#include "attacks_fwd.hpp"
#include "bitboard.hpp"
#include "board.hpp"
#include "coords.hpp"
#include "piece.hpp"

namespace chess {

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

    // c++23
#if defined(__cpp_lib_unreachable) && __cpp_lib_unreachable >= 202202L
    std::unreachable();
#endif

    assert(false);

    return {};
}

template <Color::underlying c>
[[nodiscard]] inline Bitboard attacks::pawnLeftAttacks(const Bitboard pawns) {
    return c == Color::WHITE ? (pawns << 7) & ~MASK_FILE[static_cast<int>(File::FILE_H)]
                             : (pawns >> 7) & ~MASK_FILE[static_cast<int>(File::FILE_A)];
}

template <Color::underlying c>
[[nodiscard]] inline Bitboard attacks::pawnRightAttacks(const Bitboard pawns) {
    return c == Color::WHITE ? (pawns << 9) & ~MASK_FILE[static_cast<int>(File::FILE_A)]
                             : (pawns >> 9) & ~MASK_FILE[static_cast<int>(File::FILE_H)];
}

[[nodiscard]] inline Bitboard attacks::pawn(Color c, Square sq) noexcept { return PawnAttacks[c][sq.index()]; }

[[nodiscard]] inline Bitboard attacks::knight(Square sq) noexcept { return KnightAttacks[sq.index()]; }

[[nodiscard]] inline Bitboard attacks::bishop(Square sq, Bitboard occupied) noexcept {
    return BishopTable[sq.index()].attacks[BishopTable[sq.index()](occupied)];
}

[[nodiscard]] inline Bitboard attacks::rook(Square sq, Bitboard occupied) noexcept {
    return RookTable[sq.index()].attacks[RookTable[sq.index()](occupied)];
}

[[nodiscard]] inline Bitboard attacks::queen(Square sq, Bitboard occupied) noexcept {
    return bishop(sq, occupied) | rook(sq, occupied);
}

[[nodiscard]] inline Bitboard attacks::king(Square sq) noexcept { return KingAttacks[sq.index()]; }

[[nodiscard]] inline Bitboard attacks::attackers(const Board &board, Color color, Square square) noexcept {
    const auto queens   = board.pieces(PieceType::QUEEN, color);
    const auto occupied = board.occ();

    // using the fact that if we can attack PieceType from square, they can attack us back
    auto atks = (pawn(~color, square) & board.pieces(PieceType::PAWN, color));
    atks |= (knight(square) & board.pieces(PieceType::KNIGHT, color));
    atks |= (bishop(square, occupied) & (board.pieces(PieceType::BISHOP, color) | queens));
    atks |= (rook(square, occupied) & (board.pieces(PieceType::ROOK, color) | queens));
    atks |= (king(square) & board.pieces(PieceType::KING, color));

    return atks & occupied;
}

template <PieceType::underlying pt>
[[nodiscard]] inline Bitboard attacks::slider(Square sq, Bitboard occupied) noexcept {
    static_assert(pt == PieceType::BISHOP || pt == PieceType::ROOK || pt == PieceType::QUEEN,
                  "PieceType must be a slider!");

    if constexpr (pt == PieceType::BISHOP) return bishop(sq, occupied);
    if constexpr (pt == PieceType::ROOK) return rook(sq, occupied);
    if constexpr (pt == PieceType::QUEEN) return queen(sq, occupied);
}

template <bool ISROOK>
[[nodiscard]] inline Bitboard attacks::sliderAttacks(Square sq, Bitboard occupied) noexcept {
    static constexpr int dirs[2][4][2] = {{1, 1, 1, -1, -1, -1, -1, 1}, {1, 0, 0, -1, -1, 0, 0, 1}};

    Bitboard attacks = 0ull;

    File pf = sq.file();
    Rank pr = sq.rank();

    for (int i = 0; i < 4; ++i) {
        int off_f = dirs[ISROOK][i][0];
        int off_r = dirs[ISROOK][i][1];

        File f;
        Rank r;
        for (f = pf + off_f, r = pr + off_r; Square::is_valid(r, f); f += off_f, r += off_r) {
            const auto index = Square(f, r).index();
            attacks.set(index);
            if (occupied.check(index)) break;
        }
    }

    return attacks;
}

inline void attacks::initSliders(Square sq, Magic table[], U64 magic,
                                 const std::function<Bitboard(Square, Bitboard)> &attacks) {
    // The edges of the board are not considered for the attacks
    // i.e. for the sq h7 edges will be a1-h1, a1-a8, a8-h8, ignoring the edge of the current square
    const Bitboard edges = ((Bitboard(Rank::RANK_1) | Bitboard(Rank::RANK_8)) & ~Bitboard(sq.rank())) |
                           ((Bitboard(File::FILE_A) | Bitboard(File::FILE_H)) & ~Bitboard(sq.file()));

    U64 occ = 0ULL;

    auto &table_sq = table[sq.index()];

#ifndef CHESS_USE_PEXT
    table_sq.magic = magic;
#endif
    table_sq.mask  = (attacks(sq, occ) & ~edges).getBits();
#ifndef CHESS_USE_PEXT
    table_sq.shift = 64 - Bitboard(table_sq.mask).count();
#endif

    if (sq < 64 - 1) {
        table[sq.index() + 1].attacks = table_sq.attacks + (1ull << Bitboard(table_sq.mask).count());
    }

    do {
        table_sq.attacks[table_sq(occ)] = attacks(sq, occ);
        occ                             = (occ - table_sq.mask) & table_sq.mask;
    } while (occ);
}

inline void attacks::initAttacks() {
    BishopTable[0].attacks = BishopAttacks;
    RookTable[0].attacks   = RookAttacks;

    for (int i = 0; i < 64; i++) {
        initSliders(static_cast<Square>(i), BishopTable, BishopMagics[i], sliderAttacks<false>);
        initSliders(static_cast<Square>(i), RookTable, RookMagics[i], sliderAttacks<true>);
    }
}
}  // namespace chess
