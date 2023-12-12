#pragma once

#include <array>
#include <cstdint>

#include "coords.hpp"
#include "attacks_fwd.hpp"
#include "constants.hpp"
#include "board.hpp"
#include "movegen_fwd.hpp"

namespace chess::movegen {
// force initialization of squares between
static auto init_squares_between = []() constexpr {
    // initialize squares between table
    std::array<std::array<std::uint64_t, 64>, 64> squares_between_bb{};
    std::uint64_t sqs = 0;

    for (int sq1 = 0; sq1 < 64; ++sq1) {
        for (int sq2 = 0; sq2 < 64; ++sq2) {
            sqs = (1ULL << sq1) | (1ULL << sq2);
            if (sq1 == sq2)
                squares_between_bb[sq1][sq2] = 0ull;
            else if (Square(sq1).file() == Square(sq2).file() ||
                     Square(sq1).rank() == Square(sq2).rank())
                squares_between_bb[sq1][sq2] =
                    (attacks::rook(Square(sq1), sqs) & attacks::rook(Square(sq2), sqs)).getBits();
            else if (Square(sq1).diagonalOf() == Square(sq2).diagonalOf() ||
                     Square(sq1).antiDiagonalOf() == Square(sq2).antiDiagonalOf())
                squares_between_bb[sq1][sq2] =
                    (attacks::bishop(Square(sq1), sqs) & attacks::bishop(Square(sq2), sqs))
                        .getBits();
        }
    }

    return squares_between_bb;
};

static const std::array<std::array<std::uint64_t, 64>, 64> SQUARES_BETWEEN_BB =
    init_squares_between();

/// @brief [Internal Usage] Generate the checkmask.
/// Returns a bitboard where the attacker path between the king and enemy piece is set.
/// @tparam c
/// @param board
/// @param sq
/// @param double_check
/// @return
template <Color::underlying c>
[[nodiscard]] Bitboard checkMask(const Board &board, Square sq, int &double_check) {
    Bitboard mask = 0;
    double_check  = 0;

    const auto opp_knight = board.pieces(PieceType::KNIGHT, ~c);
    const auto opp_bishop = board.pieces(PieceType::BISHOP, ~c);
    const auto opp_rook   = board.pieces(PieceType::ROOK, ~c);
    const auto opp_queen  = board.pieces(PieceType::QUEEN, ~c);

    const auto opp_pawns = board.pieces(PieceType::PAWN, ~c);

    // check for knight checks
    Bitboard knight_attacks = attacks::knight(sq) & opp_knight;
    double_check += bool(knight_attacks);

    mask |= knight_attacks;

    // check for pawn checks
    Bitboard pawn_attacks = attacks::pawn(board.sideToMove(), sq) & opp_pawns;
    mask |= pawn_attacks;
    double_check += bool(pawn_attacks);

    // check for bishop checks
    Bitboard bishop_attacks = attacks::bishop(sq, board.occ()) & (opp_bishop | opp_queen);

    if (bishop_attacks) {
        const auto index = bishop_attacks.lsb();

        mask |= SQUARES_BETWEEN_BB[sq.index()][index] | (1ULL << index);
        double_check++;
    }

    Bitboard rook_attacks = attacks::rook(sq, board.occ()) & (opp_rook | opp_queen);
    if (rook_attacks) {
        if (rook_attacks.count() > 1) {
            double_check = 2;
            return mask;
        }

        const auto index = rook_attacks.lsb();

        mask |= SQUARES_BETWEEN_BB[sq.index()][index] | (1ULL << index);
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
/// @param occ_enemy
/// @param occ_us
/// @return
template <Color::underlying c>
[[nodiscard]] Bitboard pinMaskRooks(const Board &board, Square sq, Bitboard occ_enemy,
                                    Bitboard occ_us) {
    Bitboard pin_hv = 0;

    const auto opp_rook  = board.pieces(PieceType::ROOK, ~c);
    const auto opp_queen = board.pieces(PieceType::QUEEN, ~c);

    Bitboard rook_attacks = attacks::rook(sq, occ_enemy) & (opp_rook | opp_queen);

    while (rook_attacks) {
        const auto index = rook_attacks.lsb();

        const Bitboard possible_pin = SQUARES_BETWEEN_BB[sq.index()][index] | (1ULL << index);
        if ((possible_pin & occ_us).count() == 1) pin_hv |= possible_pin;
    }

    return pin_hv;
}

/// @brief [Internal Usage] Generate the pin mask for diagonal pins.
/// Returns a bitboard where the ray between the king and the pinner is set.
/// @tparam c
/// @param board
/// @param sq
/// @param occ_enemy
/// @param occ_us
/// @return
template <Color::underlying c>
[[nodiscard]] Bitboard pinMaskBishops(const Board &board, Square sq, Bitboard occ_enemy,
                                      Bitboard occ_us) {
    Bitboard pin_diag = 0;

    const auto opp_bishop = board.pieces(PieceType::BISHOP, ~c);
    const auto opp_queen  = board.pieces(PieceType::QUEEN, ~c);

    Bitboard bishop_attacks = attacks::bishop(sq, occ_enemy) & (opp_bishop | opp_queen);

    while (bishop_attacks) {
        const auto index = bishop_attacks.pop();

        const Bitboard possible_pin = SQUARES_BETWEEN_BB[sq.index()][index] | (1ULL << index);
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
[[nodiscard]] Bitboard seenSquares(const Board &board, Bitboard enemy_empty) {
    auto king_sq = board.kingSq(~c);

    auto queens  = board.pieces(PieceType::QUEEN, c);
    auto pawns   = board.pieces(PieceType::PAWN, c);
    auto knights = board.pieces(PieceType::KNIGHT, c);
    auto bishops = board.pieces(PieceType::BISHOP, c) | queens;
    auto rooks   = board.pieces(PieceType::ROOK, c) | queens;

    auto occ = board.occ();

    Bitboard map_king_atk = attacks::king(king_sq) & enemy_empty;

    if (map_king_atk == Bitboard(0ull) && !board.chess960()) {
        return 0ull;
    }

    occ &= ~(1ULL << king_sq.index());

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
/// @param occ_enemy
template <Color::underlying c, MoveGenType mt>
void generatePawnMoves(const Board &board, Movelist &moves, Bitboard pin_d, Bitboard pin_hv,
                       Bitboard checkmask, Bitboard occ_enemy) {
    const auto pawns = board.pieces(PieceType::PAWN, c);

    constexpr Direction UP   = c == Color::WHITE ? Direction::NORTH : Direction::SOUTH;
    constexpr Direction DOWN = c == Color::WHITE ? Direction::SOUTH : Direction::NORTH;
    constexpr Direction DOWN_LEFT =
        c == Color::WHITE ? Direction::SOUTH_WEST : Direction::NORTH_EAST;
    constexpr Direction DOWN_RIGHT =
        c == Color::WHITE ? Direction::SOUTH_EAST : Direction::NORTH_WEST;

    constexpr Bitboard RANK_B_PROMO     = c == Color::WHITE
                                              ? attacks::MASK_RANK[static_cast<int>(Rank::RANK_7)]
                                              : attacks::MASK_RANK[static_cast<int>(Rank::RANK_2)];
    constexpr Bitboard RANK_PROMO       = c == Color::WHITE
                                              ? attacks::MASK_RANK[static_cast<int>(Rank::RANK_8)]
                                              : attacks::MASK_RANK[static_cast<int>(Rank::RANK_1)];
    constexpr Bitboard DOUBLE_PUSH_RANK = c == Color::WHITE
                                              ? attacks::MASK_RANK[static_cast<int>(Rank::RANK_3)]
                                              : attacks::MASK_RANK[static_cast<int>(Rank::RANK_6)];

    // These pawns can maybe take Left or Right
    const Bitboard pawns_lr = pawns & ~pin_hv;

    const Bitboard unpinnedpawns_lr = pawns_lr & ~pin_d;
    const Bitboard pinnedpawns_lr   = pawns_lr & pin_d;

    Bitboard l_pawns = (attacks::pawnLeftAttacks<c>(unpinnedpawns_lr)) |
                       (attacks::pawnLeftAttacks<c>(pinnedpawns_lr) & pin_d);

    Bitboard r_pawns = (attacks::pawnRightAttacks<c>(unpinnedpawns_lr)) |
                       (attacks::pawnRightAttacks<c>(pinnedpawns_lr) & pin_d);

    // Prune moves that don't capture a piece and are not on the checkmask.
    l_pawns &= occ_enemy & checkmask;
    r_pawns &= occ_enemy & checkmask;

    // These pawns can walk Forward
    const Bitboard pawns_hv = pawns & ~pin_d;

    const Bitboard pawns_pinned_hv   = pawns_hv & pin_hv;
    const Bitboard pawns_unpinned_hv = pawns_hv & ~pin_hv;

    // Prune moves that are blocked by a piece
    const Bitboard single_push_unpinned = attacks::shift<UP>(pawns_unpinned_hv) & ~board.occ();
    const Bitboard single_push_pinned = attacks::shift<UP>(pawns_pinned_hv) & pin_hv & ~board.occ();

    // Prune moves that are not on the checkmask.
    Bitboard single_push = (single_push_unpinned | single_push_pinned) & checkmask;

    Bitboard double_push =
        ((attacks::shift<UP>(single_push_unpinned & DOUBLE_PUSH_RANK) & ~board.occ()) |
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
        const Square epPawn = ep + DOWN;

        const Bitboard ep_mask = (1ull << epPawn.index()) | (1ull << ep.index());

        /*
         In case the en passant square and the enemy pawn
         that just moved are not on the checkmask
         en passant is not available.
        */
        if ((checkmask & ep_mask) == Bitboard(0)) return;

        const Square kSQ = board.kingSq(c);
        const Bitboard kingMask =
            (1ull << kSQ.index()) &
            attacks::MASK_RANK[static_cast<int>(epPawn.rank().internal())].getBits();
        const Bitboard enemyQueenRook =
            board.pieces(PieceType::ROOK, ~c) | board.pieces(PieceType::QUEEN, ~c);

        const bool isPossiblePin = kingMask && enemyQueenRook;
        Bitboard epBB            = attacks::pawn(~c, ep) & pawns_lr;

        // For one en passant square two pawns could potentially take there.

        while (epBB) {
            const Square from = epBB.pop();
            const Square to   = ep;

            /*
             If the pawn is pinned but the en passant square is not on the
             pin mask then the move is illegal.
            */
            if (((1ULL << from.index()) & pin_d.getBits()) &&
                !(pin_d & Bitboard(1ull << ep.index()).getBits()))
                continue;

            const Bitboard connectingPawns = (1ull << epPawn.index()) | (1ull << from.index());

            /*
             7k/4p3/8/2KP3r/8/8/8/8 b - - 0 1
             If e7e5 there will be a potential ep square for us on e6.
             However, we cannot take en passant because that would put our king
             in check. For this scenario we check if there's an enemy rook/queen
             that would give check if the two pawns were removed.
             If that's the case then the move is illegal and we can break immediately.
            */
            if (isPossiblePin && (attacks::rook(kSQ, board.occ() & ~connectingPawns) &
                                  enemyQueenRook) != Bitboard(0))
                break;

            moves.add(Move::make<Move::ENPASSANT>(from, to));
        }
    }
}

/// @brief [Internal Usage] Generate knight moves.
/// @param sq
/// @param movable
/// @return
[[nodiscard]] inline Bitboard generateKnightMoves(Square sq) { return attacks::knight(sq); }

/// @brief [Internal Usage] Generate bishop moves.
/// @param sq
/// @param movable
/// @param pin_d
/// @param occ_all
/// @return
[[nodiscard]] inline Bitboard generateBishopMoves(Square sq, Bitboard pin_d, Bitboard occ_all) {
    // The Bishop is pinned diagonally thus can only move diagonally.
    if (pin_d & Bitboard(1ULL << int(sq.internal()))) return attacks::bishop(sq, occ_all) & pin_d;
    return attacks::bishop(sq, occ_all);
}

/// @brief [Internal Usage] Generate rook moves.
/// @param sq
/// @param movable
/// @param pin_hv
/// @param occ_all
/// @return
[[nodiscard]] inline Bitboard generateRookMoves(Square sq, Bitboard pin_hv, Bitboard occ_all) {
    // The Rook is pinned horizontally thus can only move horizontally.
    if (pin_hv & Bitboard(1ULL << int(sq.internal()))) return attacks::rook(sq, occ_all) & pin_hv;
    return attacks::rook(sq, occ_all);
}

/// @brief [Internal Usage] Generate queen moves.
/// @param sq
/// @param movable
/// @param pin_d
/// @param pin_hv
/// @param occ_all
/// @return
[[nodiscard]] inline Bitboard generateQueenMoves(Square sq, Bitboard pin_d, Bitboard pin_hv,
                                                 Bitboard occ_all) {
    Bitboard moves = 0ULL;

    if (pin_d & Bitboard(1ULL << int(sq.internal())))
        moves |= attacks::bishop(sq, occ_all) & pin_d;
    else if (pin_hv & Bitboard(1ULL << int(sq.internal())))
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
[[nodiscard]] inline Bitboard generateKingMoves(Square sq, Bitboard _seen,
                                                Bitboard movable_square) {
    return attacks::king(sq) & movable_square & ~_seen;
}

/// @brief [Internal Usage] Generate castling moves.
/// @tparam c
/// @tparam mt
/// @param board
/// @param sq
/// @param seen
/// @param pinHV
/// @return
template <Color::underlying c, MoveGenType mt>
[[nodiscard]] inline Bitboard generateCastleMoves(const Board &board, Square sq, Bitboard seen,
                                                  Bitboard pinHV) {
    if constexpr (mt == MoveGenType::CAPTURE) return 0ull;
    const auto rights = board.castlingRights();

    Bitboard moves = 0ull;

    for (const auto side :
         {Board::CastlingRights::Side::KING_SIDE, Board::CastlingRights::Side::QUEEN_SIDE}) {
        if (!rights.has(c, side)) continue;

        const auto end_king_sq = utils::relativeSquare(
            c, side == Board::CastlingRights::Side::KING_SIDE ? Square::underlying::SQ_G1
                                                              : Square::underlying::SQ_C1);
        const auto end_rook_sq = utils::relativeSquare(
            c, side == Board::CastlingRights::Side::KING_SIDE ? Square::underlying::SQ_F1
                                                              : Square::underlying::SQ_D1);

        const auto from_rook_sq = Square(rights.getRookFile(c, side), sq.rank());

        const Bitboard not_occ_path      = SQUARES_BETWEEN_BB[sq.index()][from_rook_sq.index()];
        const Bitboard not_attacked_path = SQUARES_BETWEEN_BB[sq.index()][end_king_sq.index()];
        const Bitboard empty_not_attacked =
            ~seen & ~(board.occ() & Bitboard(~(1ull << from_rook_sq.index())));
        const Bitboard withoutRook = board.occ() & Bitboard(~(1ull << from_rook_sq.index()));
        const Bitboard withoutKing = board.occ() & Bitboard(~(1ull << sq.index()));

        if ((not_attacked_path & empty_not_attacked) == not_attacked_path &&
            ((not_occ_path & ~board.occ()) == not_occ_path) &&
            !((1ull << from_rook_sq.index()) & pinHV.getBits() &
              attacks::MASK_RANK[static_cast<int>(sq.rank().internal())].getBits()) &&
            !((1ull << end_rook_sq.index()) & (withoutRook & withoutKing).getBits()) &&
            !((1ull << end_king_sq.index()) &
              (seen | (withoutRook & Bitboard(~(1ull << sq.index())))).getBits())) {
            moves |= (1ull << from_rook_sq.index());
        }
    }

    return moves;
}

template <typename T>
inline void whileBitboardAdd(Movelist &movelist, Bitboard mask, T func) {
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
template <Color::underlying c, MoveGenType mt>
void legalmoves(Movelist &movelist, const Board &board, int pieces) {
    /*
     The size of the movelist might not
     be 0! This is done on purpose since it enables
     you to append new move types to any movelist.
    */
    auto king_sq = board.kingSq(c);

    int _doubleCheck = 0;

    Bitboard _occ_us    = board.us(c);
    Bitboard _occ_enemy = board.us(~c);
    Bitboard _occ_all   = _occ_us | _occ_enemy;

    Bitboard _enemy_emptyBB = ~_occ_us;

    Bitboard _checkMask = checkMask<c>(board, king_sq, _doubleCheck);
    Bitboard _pinHV     = pinMaskRooks<c>(board, king_sq, _occ_enemy, _occ_us);
    Bitboard _pinD      = pinMaskBishops<c>(board, king_sq, _occ_enemy, _occ_us);

    assert(_doubleCheck <= 2);

    // Moves have to be on the checkmask
    Bitboard movable_square;

    // Slider, Knights and King moves can only go to enemy or empty squares.
    if (mt == MoveGenType::ALL)
        movable_square = _enemy_emptyBB;
    else if (mt == MoveGenType::CAPTURE)
        movable_square = _occ_enemy;
    else  // QUIET moves
        movable_square = ~_occ_all;

    if (pieces & PieceGenType::KING) {
        Bitboard _seen = seenSquares<~c>(board, _enemy_emptyBB);

        whileBitboardAdd(movelist, 1ull << king_sq.index(),
                         [&](Square sq) { return generateKingMoves(sq, _seen, movable_square); });

        if (king_sq.rank() == (c == Color::WHITE ? Rank::RANK_1 : Rank::RANK_8) &&
            (board.castlingRights().has(c) && _checkMask == constants::DEFAULT_CHECKMASK)) {
            Bitboard moves_bb = generateCastleMoves<c, mt>(board, king_sq, _seen, _pinHV);

            while (moves_bb) {
                Square to = moves_bb.pop();
                movelist.add(Move::make<Move::CASTLING>(king_sq, to));
            }
        }
    }

    movable_square &= _checkMask;

    // Early return for double check as described earlier
    if (_doubleCheck == 2) return;

    // Add the moves to the movelist.
    if (pieces & PieceGenType::PAWN) {
        generatePawnMoves<c, mt>(board, movelist, _pinD, _pinHV, _checkMask, _occ_enemy);
    }

    if (pieces & PieceGenType::KNIGHT) {
        // Prune knights that are pinned since these cannot move.
        Bitboard knights_mask = board.pieces(PieceType::KNIGHT, c) & ~(_pinD | _pinHV);

        whileBitboardAdd(movelist, knights_mask,
                         [&](Square sq) { return generateKnightMoves(sq) & movable_square; });
    }

    if (pieces & PieceGenType::BISHOP) {
        // Prune horizontally pinned bishops
        Bitboard bishops_mask = board.pieces(PieceType::BISHOP, c) & ~_pinHV;

        whileBitboardAdd(movelist, bishops_mask, [&](Square sq) {
            return generateBishopMoves(sq, _pinD, _occ_all) & movable_square;
        });
    }

    if (pieces & PieceGenType::ROOK) {
        //  Prune diagonally pinned rooks
        Bitboard rooks_mask = board.pieces(PieceType::ROOK, c) & ~_pinD;

        whileBitboardAdd(movelist, rooks_mask, [&](Square sq) {
            return generateRookMoves(sq, _pinHV, _occ_all) & movable_square;
        });
    }

    if (pieces & PieceGenType::QUEEN) {
        // Prune double pinned queens
        Bitboard queens_mask = board.pieces(PieceType::QUEEN, c) & ~(_pinD & _pinHV);

        whileBitboardAdd(movelist, queens_mask, [&](Square sq) {
            return generateQueenMoves(sq, _pinD, _pinHV, _occ_all) & movable_square;
        });
    }
}

template <MoveGenType mt>
inline void legalmoves(Movelist &movelist, const Board &board, int pieces) {
    movelist.clear();

    if (board.sideToMove() == Color::WHITE)
        legalmoves<Color::WHITE, mt>(movelist, board, pieces);
    else
        legalmoves<Color::BLACK, mt>(movelist, board, pieces);
}
}  // namespace chess::movegen
