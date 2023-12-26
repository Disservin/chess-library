#pragma once

#include <array>
#include <cstdint>

#include "coords.hpp"
#include "attacks_fwd.hpp"
#include "constants.hpp"
#include "board.hpp"
#include "movegen_fwd.hpp"

namespace chess {

inline auto movegen::init_squares_between() {
    std::array<std::array<Bitboard, 64>, 64> squares_between_bb{};
    Bitboard sqs = 0;

    for (Square sq1 = 0; sq1 < 64; ++sq1) {
        for (Square sq2 = 0; sq2 < 64; ++sq2) {
            sqs = Bitboard::fromSquare(sq1) | Bitboard::fromSquare(sq2);
            if (sq1 == sq2)
                squares_between_bb[sq1.index()][sq2.index()].clear();
            else if (sq1.file() == sq2.file() || sq1.rank() == sq2.rank())
                squares_between_bb[sq1.index()][sq2.index()] = attacks::rook(sq1, sqs) & attacks::rook(sq2, sqs);
            else if (sq1.diagonal_of() == sq2.diagonal_of() || sq1.antidiagonal_of() == sq2.antidiagonal_of())
                squares_between_bb[sq1.index()][sq2.index()] = attacks::bishop(sq1, sqs) & attacks::bishop(sq2, sqs);
        }
    }

    return squares_between_bb;
}

/// @brief [Internal Usage] Generate the checkmask.
/// Returns a bitboard where the attacker path between the king and enemy piece is set.
/// @tparam c
/// @param board
/// @param sq
/// @param double_check
/// @return
template <Color::underlying c>
[[nodiscard]] inline Bitboard movegen::checkMask(const Board &board, Square sq, int &double_check) {
    double_check = 0;

    const auto opp_knight = board.pieces(PieceType::KNIGHT, ~c);
    const auto opp_bishop = board.pieces(PieceType::BISHOP, ~c);
    const auto opp_rook   = board.pieces(PieceType::ROOK, ~c);
    const auto opp_queen  = board.pieces(PieceType::QUEEN, ~c);

    const auto opp_pawns = board.pieces(PieceType::PAWN, ~c);

    // check for knight checks
    Bitboard knight_attacks = attacks::knight(sq) & opp_knight;
    double_check += bool(knight_attacks);

    Bitboard mask = knight_attacks;

    // check for pawn checks
    Bitboard pawn_attacks = attacks::pawn(board.sideToMove(), sq) & opp_pawns;
    mask |= pawn_attacks;
    double_check += bool(pawn_attacks);

    // check for bishop checks
    Bitboard bishop_attacks = attacks::bishop(sq, board.occ()) & (opp_bishop | opp_queen);

    if (bishop_attacks) {
        const auto index = bishop_attacks.lsb();

        mask |= SQUARES_BETWEEN_BB[sq.index()][index] | Bitboard::fromSquare(index);
        double_check++;
    }

    Bitboard rook_attacks = attacks::rook(sq, board.occ()) & (opp_rook | opp_queen);
    if (rook_attacks) {
        if (rook_attacks.count() > 1) {
            double_check = 2;
            return mask;
        }

        const auto index = rook_attacks.lsb();

        mask |= SQUARES_BETWEEN_BB[sq.index()][index] | Bitboard::fromSquare(index);
        double_check++;
    }

    if (!mask) {
        return constants::DEFAULT_CHECKMASK;
    }

    return mask;
}

/// @brief [Internal Usage] Generate the pin mask for horizontal and vertical pins.
/// Returns a bitboard where the ray between the king and the pinner is set.
/// @tparam c
/// @param board
/// @param sq
/// @param occ_opp
/// @param occ_us
/// @return
template <Color::underlying c>
[[nodiscard]] inline Bitboard movegen::pinMaskRooks(const Board &board, Square sq, Bitboard occ_opp, Bitboard occ_us) {
    const auto opp_rook  = board.pieces(PieceType::ROOK, ~c);
    const auto opp_queen = board.pieces(PieceType::QUEEN, ~c);

    Bitboard rook_attacks = attacks::rook(sq, occ_opp) & (opp_rook | opp_queen);
    Bitboard pin_hv       = 0;

    while (rook_attacks) {
        const auto index = rook_attacks.pop();

        const Bitboard possible_pin = SQUARES_BETWEEN_BB[sq.index()][index] | Bitboard::fromSquare(index);
        if ((possible_pin & occ_us).count() == 1) pin_hv |= possible_pin;
    }

    return pin_hv;
}

/// @brief [Internal Usage] Generate the pin mask for diagonal pins.
/// Returns a bitboard where the ray between the king and the pinner is set.
/// @tparam c
/// @param board
/// @param sq
/// @param occ_opp
/// @param occ_us
/// @return
template <Color::underlying c>
[[nodiscard]] inline Bitboard movegen::pinMaskBishops(const Board &board, Square sq, Bitboard occ_opp,
                                                      Bitboard occ_us) {
    const auto opp_bishop = board.pieces(PieceType::BISHOP, ~c);
    const auto opp_queen  = board.pieces(PieceType::QUEEN, ~c);

    Bitboard bishop_attacks = attacks::bishop(sq, occ_opp) & (opp_bishop | opp_queen);
    Bitboard pin_diag       = 0;

    while (bishop_attacks) {
        const auto index = bishop_attacks.pop();

        const Bitboard possible_pin = SQUARES_BETWEEN_BB[sq.index()][index] | Bitboard::fromSquare(index);
        if ((possible_pin & occ_us).count() == 1) pin_diag |= possible_pin;
    }

    return pin_diag;
}

/// @brief [Internal Usage] Returns the squares that are attacked by the enemy
/// @tparam c
/// @param board
/// @param enemy_empty
/// @return
template <Color::underlying c>
[[nodiscard]] inline Bitboard movegen::seenSquares(const Board &board, Bitboard enemy_empty) {
    auto king_sq          = board.kingSq(~c);
    Bitboard map_king_atk = attacks::king(king_sq) & enemy_empty;

    if (map_king_atk == Bitboard(0ull) && !board.chess960()) {
        return 0ull;
    }

    auto occ     = board.occ() & ~Bitboard::fromSquare(king_sq);
    auto queens  = board.pieces(PieceType::QUEEN, c);
    auto pawns   = board.pieces(PieceType::PAWN, c);
    auto knights = board.pieces(PieceType::KNIGHT, c);
    auto bishops = board.pieces(PieceType::BISHOP, c) | queens;
    auto rooks   = board.pieces(PieceType::ROOK, c) | queens;

    Bitboard seen = attacks::pawnLeftAttacks<c>(pawns) | attacks::pawnRightAttacks<c>(pawns);

    while (knights) {
        const auto index = knights.pop();
        seen |= attacks::knight(index);
    }

    while (bishops) {
        const auto index = bishops.pop();
        seen |= attacks::bishop(index, occ);
    }

    while (rooks) {
        const auto index = rooks.pop();
        seen |= attacks::rook(index, occ);
    }

    const Square index = board.kingSq(c);
    seen |= attacks::king(index);

    return seen;
}

/// @brief [Internal Usage] Generate pawn moves.
/// @tparam c
/// @tparam mt
/// @param board
/// @param moves
/// @param pin_d
/// @param pin_hv
/// @param checkmask
/// @param occ_opp
template <Color::underlying c, movegen::MoveGenType mt>
inline void movegen::generatePawnMoves(const Board &board, Movelist &moves, Bitboard pin_d, Bitboard pin_hv,
                                       Bitboard checkmask, Bitboard occ_opp) {
    constexpr Direction UP              = c == Color::WHITE ? Direction::NORTH : Direction::SOUTH;
    constexpr Direction DOWN            = c == Color::WHITE ? Direction::SOUTH : Direction::NORTH;
    constexpr Direction DOWN_LEFT       = c == Color::WHITE ? Direction::SOUTH_WEST : Direction::NORTH_EAST;
    constexpr Direction DOWN_RIGHT      = c == Color::WHITE ? Direction::SOUTH_EAST : Direction::NORTH_WEST;
    constexpr Bitboard RANK_B_PROMO     = c == Color::WHITE ? attacks::MASK_RANK[static_cast<int>(Rank::RANK_7)]
                                                            : attacks::MASK_RANK[static_cast<int>(Rank::RANK_2)];
    constexpr Bitboard RANK_PROMO       = c == Color::WHITE ? attacks::MASK_RANK[static_cast<int>(Rank::RANK_8)]
                                                            : attacks::MASK_RANK[static_cast<int>(Rank::RANK_1)];
    constexpr Bitboard DOUBLE_PUSH_RANK = c == Color::WHITE ? attacks::MASK_RANK[static_cast<int>(Rank::RANK_3)]
                                                            : attacks::MASK_RANK[static_cast<int>(Rank::RANK_6)];

    const auto pawns = board.pieces(PieceType::PAWN, c);

    // These pawns can maybe take Left or Right
    const Bitboard pawns_lr         = pawns & ~pin_hv;
    const Bitboard unpinnedpawns_lr = pawns_lr & ~pin_d;
    const Bitboard pinnedpawns_lr   = pawns_lr & pin_d;

    Bitboard l_pawns =
        attacks::pawnLeftAttacks<c>(unpinnedpawns_lr) | (attacks::pawnLeftAttacks<c>(pinnedpawns_lr) & pin_d);

    Bitboard r_pawns =
        attacks::pawnRightAttacks<c>(unpinnedpawns_lr) | (attacks::pawnRightAttacks<c>(pinnedpawns_lr) & pin_d);

    // Prune moves that don't capture a piece and are not on the checkmask.
    l_pawns &= occ_opp & checkmask;
    r_pawns &= occ_opp & checkmask;

    // These pawns can walk Forward
    const Bitboard pawns_hv = pawns & ~pin_d;

    const Bitboard pawns_pinned_hv   = pawns_hv & pin_hv;
    const Bitboard pawns_unpinned_hv = pawns_hv & ~pin_hv;

    // Prune moves that are blocked by a piece
    const Bitboard single_push_unpinned = attacks::shift<UP>(pawns_unpinned_hv) & ~board.occ();
    const Bitboard single_push_pinned   = attacks::shift<UP>(pawns_pinned_hv) & pin_hv & ~board.occ();

    // Prune moves that are not on the checkmask.
    Bitboard single_push = (single_push_unpinned | single_push_pinned) & checkmask;

    Bitboard double_push = ((attacks::shift<UP>(single_push_unpinned & DOUBLE_PUSH_RANK) & ~board.occ()) |
                            (attacks::shift<UP>(single_push_pinned & DOUBLE_PUSH_RANK) & ~board.occ())) &
                           checkmask;

    if (pawns & RANK_B_PROMO) {
        Bitboard promo_left  = l_pawns & RANK_PROMO;
        Bitboard promo_right = r_pawns & RANK_PROMO;
        Bitboard promo_push  = single_push & RANK_PROMO;

        // Skip capturing promotions if we are only generating quiet moves.
        // Generates at ALL and CAPTURE
        while (mt != MoveGenType::QUIET && promo_left) {
            const auto index = promo_left.pop();
            moves.add(Move::make<Move::PROMOTION>(index + DOWN_RIGHT, index, PieceType::QUEEN));
            moves.add(Move::make<Move::PROMOTION>(index + DOWN_RIGHT, index, PieceType::ROOK));
            moves.add(Move::make<Move::PROMOTION>(index + DOWN_RIGHT, index, PieceType::BISHOP));
            moves.add(Move::make<Move::PROMOTION>(index + DOWN_RIGHT, index, PieceType::KNIGHT));
        }

        // Skip capturing promotions if we are only generating quiet moves.
        // Generates at ALL and CAPTURE
        while (mt != MoveGenType::QUIET && promo_right) {
            const auto index = promo_right.pop();
            moves.add(Move::make<Move::PROMOTION>(index + DOWN_LEFT, index, PieceType::QUEEN));
            moves.add(Move::make<Move::PROMOTION>(index + DOWN_LEFT, index, PieceType::ROOK));
            moves.add(Move::make<Move::PROMOTION>(index + DOWN_LEFT, index, PieceType::BISHOP));
            moves.add(Move::make<Move::PROMOTION>(index + DOWN_LEFT, index, PieceType::KNIGHT));
        }

        // Skip quiet promotions if we are only generating captures.
        // Generates at ALL and QUIET
        while (mt != MoveGenType::CAPTURE && promo_push) {
            const auto index = promo_push.pop();
            moves.add(Move::make<Move::PROMOTION>(index + DOWN, index, PieceType::QUEEN));
            moves.add(Move::make<Move::PROMOTION>(index + DOWN, index, PieceType::ROOK));
            moves.add(Move::make<Move::PROMOTION>(index + DOWN, index, PieceType::BISHOP));
            moves.add(Move::make<Move::PROMOTION>(index + DOWN, index, PieceType::KNIGHT));
        }
    }

    single_push &= ~RANK_PROMO;
    l_pawns &= ~RANK_PROMO;
    r_pawns &= ~RANK_PROMO;

    while (mt != MoveGenType::QUIET && l_pawns) {
        const auto index = l_pawns.pop();
        moves.add(Move::make<Move::NORMAL>(index + DOWN_RIGHT, index));
    }

    while (mt != MoveGenType::QUIET && r_pawns) {
        const auto index = r_pawns.pop();
        moves.add(Move::make<Move::NORMAL>(index + DOWN_LEFT, index));
    }

    while (mt != MoveGenType::CAPTURE && single_push) {
        const auto index = single_push.pop();
        moves.add(Move::make<Move::NORMAL>(index + DOWN, index));
    }

    while (mt != MoveGenType::CAPTURE && double_push) {
        const auto index = double_push.pop();
        moves.add(Move::make<Move::NORMAL>(index + DOWN + DOWN, index));
    }

    const Square ep = board.enpassantSq();
    if (mt != MoveGenType::QUIET && ep != Square::underlying::NO_SQ) {
        const Square epPawn    = ep + DOWN;
        const Bitboard ep_mask = Bitboard::fromSquare(epPawn) | Bitboard::fromSquare(ep);

        /*
         In case the en passant square and the enemy pawn
         that just moved are not on the checkmask
         en passant is not available.
        */
        if ((checkmask & ep_mask) == Bitboard(0)) return;

        const Square kSQ              = board.kingSq(c);
        const Bitboard kingMask       = Bitboard::fromSquare(kSQ) & attacks::MASK_RANK[epPawn.rank()].getBits();
        const Bitboard enemyQueenRook = board.pieces(PieceType::ROOK, ~c) | board.pieces(PieceType::QUEEN, ~c);

        Bitboard epBB = attacks::pawn(~c, ep) & pawns_lr;

        // For one en passant square two pawns could potentially take there.
        while (epBB) {
            const Square from = epBB.pop();
            const Square to   = ep;

            /*
             If the pawn is pinned but the en passant square is not on the
             pin mask then the move is illegal.
            */
            if (static_cast<bool>((Bitboard::fromSquare(from) & pin_d.getBits())) &&
                !(pin_d & Bitboard::fromSquare(ep).getBits()))
                continue;

            const Bitboard connectingPawns = Bitboard::fromSquare(epPawn) | Bitboard::fromSquare(from);

            /*
             7k/4p3/8/2KP3r/8/8/8/8 b - - 0 1
             If e7e5 there will be a potential ep square for us on e6.
             However, we cannot take en passant because that would put our king
             in check. For this scenario we check if there's an enemy rook/queen
             that would give check if the two pawns were removed.
             If that's the case then the move is illegal and we can break immediately.
            */
            const bool isPossiblePin = kingMask && enemyQueenRook;
            if (isPossiblePin && (attacks::rook(kSQ, board.occ() & ~connectingPawns) & enemyQueenRook) != Bitboard(0))
                break;

            moves.add(Move::make<Move::ENPASSANT>(from, to));
        }
    }
}

/// @brief [Internal Usage] Generate knight moves.
/// @param sq
/// @param movable
/// @return
[[nodiscard]] inline Bitboard movegen::generateKnightMoves(Square sq) { return attacks::knight(sq); }

/// @brief [Internal Usage] Generate bishop moves.
/// @param sq
/// @param movable
/// @param pin_d
/// @param occ_all
/// @return
[[nodiscard]] inline Bitboard movegen::generateBishopMoves(Square sq, Bitboard pin_d, Bitboard occ_all) {
    // The Bishop is pinned diagonally thus can only move diagonally.
    if (pin_d & Bitboard::fromSquare(sq)) return attacks::bishop(sq, occ_all) & pin_d;
    return attacks::bishop(sq, occ_all);
}

/// @brief [Internal Usage] Generate rook moves.
/// @param sq
/// @param movable
/// @param pin_hv
/// @param occ_all
/// @return
[[nodiscard]] inline Bitboard movegen::generateRookMoves(Square sq, Bitboard pin_hv, Bitboard occ_all) {
    // The Rook is pinned horizontally thus can only move horizontally.
    if (pin_hv & Bitboard::fromSquare(sq)) return attacks::rook(sq, occ_all) & pin_hv;
    return attacks::rook(sq, occ_all);
}

/// @brief [Internal Usage] Generate queen moves.
/// @param sq
/// @param movable
/// @param pin_d
/// @param pin_hv
/// @param occ_all
/// @return
[[nodiscard]] inline Bitboard movegen::generateQueenMoves(Square sq, Bitboard pin_d, Bitboard pin_hv,
                                                          Bitboard occ_all) {
    Bitboard moves = 0ULL;

    if (pin_d & Bitboard::fromSquare(sq))
        moves |= attacks::bishop(sq, occ_all) & pin_d;
    else if (pin_hv & Bitboard::fromSquare(sq))
        moves |= attacks::rook(sq, occ_all) & pin_hv;
    else {
        moves |= attacks::rook(sq, occ_all);
        moves |= attacks::bishop(sq, occ_all);
    }

    return moves;
}

/// @brief [Internal Usage] Generate king moves.
/// @param sq
/// @param _seen
/// @param movable_square
/// @return
[[nodiscard]] inline Bitboard movegen::generateKingMoves(Square sq, Bitboard _seen, Bitboard movable_square) {
    return attacks::king(sq) & movable_square & ~_seen;
}

/// @brief [Internal Usage] Generate castling moves.
/// @tparam c
/// @tparam mt
/// @param board
/// @param sq
/// @param seen
/// @param pin_hv
/// @return
template <Color::underlying c, movegen::MoveGenType mt>
[[nodiscard]] inline Bitboard movegen::generateCastleMoves(const Board &board, Square sq, Bitboard seen,
                                                           Bitboard pin_hv) {
    if constexpr (mt == MoveGenType::CAPTURE) return 0ull;
    const auto rights = board.castlingRights();

    Bitboard moves = 0ull;

    for (const auto side : {Board::CastlingRights::Side::KING_SIDE, Board::CastlingRights::Side::QUEEN_SIDE}) {
        if (!rights.has(c, side)) continue;

        const auto end_king_sq = Square::castling_king_square(side == Board::CastlingRights::Side::KING_SIDE, c);
        const auto end_rook_sq = Square::castling_rook_square(side == Board::CastlingRights::Side::KING_SIDE, c);

        const auto from_rook_sq = Square(rights.getRookFile(c, side), sq.rank());

        const Bitboard not_occ_path       = SQUARES_BETWEEN_BB[sq.index()][from_rook_sq.index()];
        const Bitboard not_attacked_path  = SQUARES_BETWEEN_BB[sq.index()][end_king_sq.index()];
        const Bitboard empty_not_attacked = ~seen & ~(board.occ() & Bitboard(~Bitboard::fromSquare(from_rook_sq)));
        const Bitboard withoutRook        = board.occ() & Bitboard(~Bitboard::fromSquare(from_rook_sq));
        const Bitboard withoutKing        = board.occ() & Bitboard(~Bitboard::fromSquare(sq));

        if ((not_attacked_path & empty_not_attacked) == not_attacked_path &&
            ((not_occ_path & ~board.occ()) == not_occ_path) &&
            !(Bitboard::fromSquare(from_rook_sq) & pin_hv.getBits() & attacks::MASK_RANK[sq.rank()].getBits()) &&
            !(Bitboard::fromSquare(end_rook_sq) & (withoutRook & withoutKing).getBits()) &&
            !(Bitboard::fromSquare(end_king_sq) &
              (seen | (withoutRook & Bitboard(~Bitboard::fromSquare(sq)))).getBits())) {
            moves |= Bitboard::fromSquare(from_rook_sq);
        }
    }

    return moves;
}

template <typename T>
inline void movegen::whileBitboardAdd(Movelist &movelist, Bitboard mask, T func) {
    while (mask) {
        const Square from = mask.pop();
        auto moves        = func(from);
        while (moves) {
            const Square to = moves.pop();
            movelist.add(Move::make<Move::NORMAL>(from, to));
        }
    }
}

/// @brief [Internal Usage] all legal moves for a position
/// @tparam c
/// @tparam mt
/// @param movelist
/// @param board
template <Color::underlying c, movegen::MoveGenType mt>
inline void movegen::legalmoves(Movelist &movelist, const Board &board, int pieces) {
    /*
     The size of the movelist might not
     be 0! This is done on purpose since it enables
     you to append new move types to any movelist.
    */
    auto king_sq = board.kingSq(c);

    int double_check = 0;

    Bitboard occ_us  = board.us(c);
    Bitboard occ_opp = board.us(~c);
    Bitboard occ_all = occ_us | occ_opp;

    Bitboard opp_empty = ~occ_us;

    Bitboard check_mask = checkMask<c>(board, king_sq, double_check);
    Bitboard pin_hv     = pinMaskRooks<c>(board, king_sq, occ_opp, occ_us);
    Bitboard pin_d      = pinMaskBishops<c>(board, king_sq, occ_opp, occ_us);

    assert(double_check <= 2);

    // Moves have to be on the checkmask
    Bitboard movable_square;

    // Slider, Knights and King moves can only go to enemy or empty squares.
    if (mt == MoveGenType::ALL)
        movable_square = opp_empty;
    else if (mt == MoveGenType::CAPTURE)
        movable_square = occ_opp;
    else  // QUIET moves
        movable_square = ~occ_all;

    if (pieces & PieceGenType::KING) {
        Bitboard _seen = seenSquares<~c>(board, opp_empty);

        whileBitboardAdd(movelist, Bitboard::fromSquare(king_sq),
                         [&](Square sq) { return generateKingMoves(sq, _seen, movable_square); });

        if (check_mask == constants::DEFAULT_CHECKMASK && Square::back_rank(king_sq, c) &&
            board.castlingRights().has(c)) {
            Bitboard moves_bb = generateCastleMoves<c, mt>(board, king_sq, _seen, pin_hv);
            while (moves_bb) {
                Square to = moves_bb.pop();
                movelist.add(Move::make<Move::CASTLING>(king_sq, to));
            }
        }
    }

    movable_square &= check_mask;

    // Early return for double check as described earlier
    if (double_check == 2) return;

    // Add the moves to the movelist.
    if (pieces & PieceGenType::PAWN) {
        generatePawnMoves<c, mt>(board, movelist, pin_d, pin_hv, check_mask, occ_opp);
    }

    if (pieces & PieceGenType::KNIGHT) {
        // Prune knights that are pinned since these cannot move.
        Bitboard knights_mask = board.pieces(PieceType::KNIGHT, c) & ~(pin_d | pin_hv);

        whileBitboardAdd(movelist, knights_mask, [&](Square sq) { return generateKnightMoves(sq) & movable_square; });
    }

    if (pieces & PieceGenType::BISHOP) {
        // Prune horizontally pinned bishops
        Bitboard bishops_mask = board.pieces(PieceType::BISHOP, c) & ~pin_hv;

        whileBitboardAdd(movelist, bishops_mask,
                         [&](Square sq) { return generateBishopMoves(sq, pin_d, occ_all) & movable_square; });
    }

    if (pieces & PieceGenType::ROOK) {
        //  Prune diagonally pinned rooks
        Bitboard rooks_mask = board.pieces(PieceType::ROOK, c) & ~pin_d;

        whileBitboardAdd(movelist, rooks_mask,
                         [&](Square sq) { return generateRookMoves(sq, pin_hv, occ_all) & movable_square; });
    }

    if (pieces & PieceGenType::QUEEN) {
        // Prune double pinned queens
        Bitboard queens_mask = board.pieces(PieceType::QUEEN, c) & ~(pin_d & pin_hv);

        whileBitboardAdd(movelist, queens_mask,
                         [&](Square sq) { return generateQueenMoves(sq, pin_d, pin_hv, occ_all) & movable_square; });
    }
}

template <movegen::MoveGenType mt>
inline void movegen::legalmoves(Movelist &movelist, const Board &board, int pieces) {
    movelist.clear();

    if (board.sideToMove() == Color::WHITE)
        legalmoves<Color::WHITE, mt>(movelist, board, pieces);
    else
        legalmoves<Color::BLACK, mt>(movelist, board, pieces);
}

inline const std::array<std::array<Bitboard, 64>, 64> movegen::SQUARES_BETWEEN_BB = movegen::init_squares_between();

}  // namespace chess
