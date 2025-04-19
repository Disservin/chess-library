#pragma once

#include <array>

#include "attacks_fwd.hpp"
#include "board.hpp"
#include "constants.hpp"
#include "coords.hpp"
#include "movegen_fwd.hpp"

namespace chess {

inline auto movegen::init_squares_between() {
    std::array<std::array<Bitboard, 64>, 64> squares_between_bb{};

    auto att = [](PieceType pt, Square sq, Bitboard occ) {
        return (pt == PieceType::BISHOP) ? attacks::bishop(sq, occ) : attacks::rook(sq, occ);
    };

    for (int sq1 = 0; sq1 < 64; ++sq1) {
        for (PieceType pt : {PieceType::BISHOP, PieceType::ROOK}) {
            for (int sq2 = 0; sq2 < 64; ++sq2) {
                if (att(pt, sq1, 0).check(sq2)) {
                    squares_between_bb[sq1][sq2] =
                        att(pt, sq1, Bitboard::fromSquare(sq2)) & att(pt, sq2, Bitboard::fromSquare(sq1));
                }
                squares_between_bb[sq1][sq2].set(sq2);
            }
        }
    }

    return squares_between_bb;
}

template <Color::underlying c>
[[nodiscard]] inline std::pair<Bitboard, int> movegen::checkMask(const Board &board, Square sq) {
    const auto opp_knight = board.pieces(PieceType::KNIGHT, ~c);
    const auto opp_bishop = board.pieces(PieceType::BISHOP, ~c);
    const auto opp_rook   = board.pieces(PieceType::ROOK, ~c);
    const auto opp_queen  = board.pieces(PieceType::QUEEN, ~c);

    const auto opp_pawns = board.pieces(PieceType::PAWN, ~c);

    int checks = 0;

    // check for knight checks
    Bitboard knight_attacks = attacks::knight(sq) & opp_knight;
    checks += bool(knight_attacks);

    Bitboard mask = knight_attacks;

    // check for pawn checks
    Bitboard pawn_attacks = attacks::pawn(board.sideToMove(), sq) & opp_pawns;
    mask |= pawn_attacks;
    checks += bool(pawn_attacks);

    // check for bishop checks
    Bitboard bishop_attacks = attacks::bishop(sq, board.occ()) & (opp_bishop | opp_queen);

    if (bishop_attacks) {
        mask |= between(sq, bishop_attacks.lsb());
        checks++;
    }

    Bitboard rook_attacks = attacks::rook(sq, board.occ()) & (opp_rook | opp_queen);

    if (rook_attacks) {
        if (rook_attacks.count() > 1) {
            checks = 2;
            return {mask, checks};
        }

        mask |= between(sq, rook_attacks.lsb());
        checks++;
    }

    if (!mask) {
        return {constants::DEFAULT_CHECKMASK, checks};
    }

    return {mask, checks};
}

template <Color::underlying c, PieceType::underlying pt>
[[nodiscard]] inline Bitboard movegen::pinMask(const Board &board, Square sq, Bitboard occ_opp,
                                               Bitboard occ_us) noexcept {
    static_assert(pt == PieceType::BISHOP || pt == PieceType::ROOK, "Only bishop or rook allowed!");

    const auto opp_pt_queen = board.pieces(pt, PieceType::QUEEN) & board.us(~c);

    auto pt_attacks = attacks::slider<pt>(sq, occ_opp) & opp_pt_queen;

    Bitboard pin = 0ull;

    while (pt_attacks) {
        const auto possible_pin = between(sq, pt_attacks.pop());
        if ((possible_pin & occ_us).count() == 1) pin |= possible_pin;
    }

    return pin;
}

template <Color::underlying c>
[[nodiscard]] inline Bitboard movegen::seenSquares(const Board &board, Bitboard enemy_empty) {
    auto king_sq          = board.kingSq(~c);
    Bitboard map_king_atk = attacks::king(king_sq) & enemy_empty;

    if (map_king_atk == Bitboard(0ull) && !board.chess960()) return 0ull;

    auto occ     = board.occ() ^ Bitboard::fromSquare(king_sq);
    auto queens  = board.pieces(PieceType::QUEEN, c);
    auto pawns   = board.pieces(PieceType::PAWN, c);
    auto knights = board.pieces(PieceType::KNIGHT, c);
    auto bishops = board.pieces(PieceType::BISHOP, c) | queens;
    auto rooks   = board.pieces(PieceType::ROOK, c) | queens;

    Bitboard seen = attacks::pawnLeftAttacks<c>(pawns) | attacks::pawnRightAttacks<c>(pawns);

    while (knights) {
        seen |= attacks::knight(knights.pop());
    }

    while (bishops) {
        seen |= attacks::bishop(bishops.pop(), occ);
    }

    while (rooks) {
        seen |= attacks::rook(rooks.pop(), occ);
    }

    seen |= attacks::king(board.kingSq(c));

    return seen;
}

template <Color::underlying c, movegen::MoveGenType mt>
inline void movegen::generatePawnMoves(const Board &board, Movelist &moves, Bitboard pin_d, Bitboard pin_hv,
                                       Bitboard checkmask, Bitboard occ_opp) {
    // flipped for black

    constexpr auto UP         = make_direction(Direction::NORTH, c);
    constexpr auto DOWN       = make_direction(Direction::SOUTH, c);
    constexpr auto DOWN_LEFT  = make_direction(Direction::SOUTH_WEST, c);
    constexpr auto DOWN_RIGHT = make_direction(Direction::SOUTH_EAST, c);
    constexpr auto UP_LEFT    = make_direction(Direction::NORTH_WEST, c);
    constexpr auto UP_RIGHT   = make_direction(Direction::NORTH_EAST, c);

    constexpr auto RANK_B_PROMO     = Rank::rank(Rank::RANK_7, c).bb();
    constexpr auto RANK_PROMO       = Rank::rank(Rank::RANK_8, c).bb();
    constexpr auto DOUBLE_PUSH_RANK = Rank::rank(Rank::RANK_3, c).bb();

    const auto pawns = board.pieces(PieceType::PAWN, c);

    // These pawns can maybe take Left or Right
    const Bitboard pawns_lr          = pawns & ~pin_hv;
    const Bitboard unpinned_pawns_lr = pawns_lr & ~pin_d;
    const Bitboard pinned_pawns_lr   = pawns_lr & pin_d;

    auto l_pawns = attacks::shift<UP_LEFT>(unpinned_pawns_lr) | (attacks::shift<UP_LEFT>(pinned_pawns_lr) & pin_d);
    auto r_pawns = attacks::shift<UP_RIGHT>(unpinned_pawns_lr) | (attacks::shift<UP_RIGHT>(pinned_pawns_lr) & pin_d);

    // Prune moves that don't capture a piece and are not on the checkmask.
    l_pawns &= occ_opp & checkmask;
    r_pawns &= occ_opp & checkmask;

    // These pawns can walk Forward
    const auto pawns_hv = pawns & ~pin_d;

    const auto pawns_pinned_hv   = pawns_hv & pin_hv;
    const auto pawns_unpinned_hv = pawns_hv & ~pin_hv;

    // Prune moves that are blocked by a piece
    const auto single_push_unpinned = attacks::shift<UP>(pawns_unpinned_hv) & ~board.occ();
    const auto single_push_pinned   = attacks::shift<UP>(pawns_pinned_hv) & pin_hv & ~board.occ();

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

    if constexpr (mt == MoveGenType::QUIET) return;

    const Square ep = board.enpassantSq();

    if (ep != Square::NO_SQ) {
        auto m = generateEPMove(board, checkmask, pin_d, pawns_lr, ep, c);

        for (const auto &move : m) {
            if (move != Move::NO_MOVE) moves.add(move);
        }
    }
}

[[nodiscard]] inline std::array<Move, 2> movegen::generateEPMove(const Board &board, Bitboard checkmask, Bitboard pin_d,
                                                                 Bitboard pawns_lr, Square ep, Color c) {
    assert((ep.rank() == Rank::RANK_3 && board.sideToMove() == Color::BLACK) ||
           (ep.rank() == Rank::RANK_6 && board.sideToMove() == Color::WHITE));

    std::array<Move, 2> moves = {Move::NO_MOVE, Move::NO_MOVE};
    int i                     = 0;

    const auto DOWN     = make_direction(Direction::SOUTH, c);
    const auto epPawnSq = ep + DOWN;

    /*
     In case the en passant square and the enemy pawn
     that just moved are not on the checkmask
     en passant is not available.
    */
    if ((checkmask & (Bitboard::fromSquare(epPawnSq) | Bitboard::fromSquare(ep))) == 0ull) return moves;

    const Square kSQ              = board.kingSq(c);
    const Bitboard kingMask       = Bitboard::fromSquare(kSQ) & epPawnSq.rank().bb();
    const Bitboard enemyQueenRook = board.pieces(PieceType::ROOK, PieceType::QUEEN) & board.us(~c);

    auto epBB = attacks::pawn(~c, ep) & pawns_lr;

    // For one en passant square two pawns could potentially take there.
    while (epBB) {
        const auto from = epBB.pop();
        const auto to   = ep;

        /*
         If the pawn is pinned but the en passant square is not on the
         pin mask then the move is illegal.
        */
        if ((Bitboard::fromSquare(from) & pin_d) && !(pin_d & Bitboard::fromSquare(ep))) continue;

        const auto connectingPawns = Bitboard::fromSquare(epPawnSq) | Bitboard::fromSquare(from);

        /*
         7k/4p3/8/2KP3r/8/8/8/8 b - - 0 1
         If e7e5 there will be a potential ep square for us on e6.
         However, we cannot take en passant because that would put our king
         in check. For this scenario we check if there's an enemy rook/queen
         that would give check if the two pawns were removed.
         If that's the case then the move is illegal and we can break immediately.
        */
        const auto isPossiblePin = kingMask && enemyQueenRook;

        if (isPossiblePin && (attacks::rook(kSQ, board.occ() ^ connectingPawns) & enemyQueenRook) != 0ull) break;

        moves[i++] = Move::make<Move::ENPASSANT>(from, to);
    }

    return moves;
}

[[nodiscard]] inline Bitboard movegen::generateKnightMoves(Square sq) { return attacks::knight(sq); }

[[nodiscard]] inline Bitboard movegen::generateBishopMoves(Square sq, Bitboard pin_d, Bitboard occ_all) {
    // The Bishop is pinned diagonally thus can only move diagonally.
    if (pin_d & Bitboard::fromSquare(sq)) return attacks::bishop(sq, occ_all) & pin_d;
    return attacks::bishop(sq, occ_all);
}

[[nodiscard]] inline Bitboard movegen::generateRookMoves(Square sq, Bitboard pin_hv, Bitboard occ_all) {
    // The Rook is pinned horizontally thus can only move horizontally.
    if (pin_hv & Bitboard::fromSquare(sq)) return attacks::rook(sq, occ_all) & pin_hv;
    return attacks::rook(sq, occ_all);
}

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

[[nodiscard]] inline Bitboard movegen::generateKingMoves(Square sq, Bitboard seen, Bitboard movable_square) {
    return attacks::king(sq) & movable_square & ~seen;
}

template <Color::underlying c>
[[nodiscard]] inline Bitboard movegen::generateCastleMoves(const Board &board, Square sq, Bitboard seen,
                                                           Bitboard pin_hv) noexcept {
    if (!Square::back_rank(sq, c) || !board.castlingRights().has(c)) return 0ull;

    const auto rights = board.castlingRights();

    Bitboard moves = 0ull;

    for (const auto side : {Board::CastlingRights::Side::KING_SIDE, Board::CastlingRights::Side::QUEEN_SIDE}) {
        if (!rights.has(c, side)) continue;

        const auto is_king_side = side == Board::CastlingRights::Side::KING_SIDE;

        // No pieces on the castling path
        if (board.occ() & board.getCastlingPath(c, is_king_side)) continue;

        // No attacks on the king path
        const auto king_to = Square::castling_king_square(is_king_side, c);
        if (between(sq, king_to) & seen) continue;

        // Chess960: Rook is pinned on the backrank.
        const auto from_rook_bb = Bitboard::fromSquare(Square(rights.getRookFile(c, side), sq.rank()));
        if (board.chess960() && (pin_hv & board.us(board.sideToMove()) & from_rook_bb)) continue;

        moves |= from_rook_bb;
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

template <Color::underlying c, movegen::MoveGenType mt>
inline void movegen::legalmoves(Movelist &movelist, const Board &board, int pieces) {
    /*
     The size of the movelist might not
     be 0! This is done on purpose since it enables
     you to append new move types to any movelist.
    */
    auto king_sq = board.kingSq(c);

    Bitboard occ_us  = board.us(c);
    Bitboard occ_opp = board.us(~c);
    Bitboard occ_all = occ_us | occ_opp;

    Bitboard opp_empty = ~occ_us;

    const auto [checkmask, checks] = checkMask<c>(board, king_sq);
    const auto pin_hv              = pinMask<c, PieceType::ROOK>(board, king_sq, occ_opp, occ_us);
    const auto pin_d               = pinMask<c, PieceType::BISHOP>(board, king_sq, occ_opp, occ_us);

    assert(checks <= 2);

    Bitboard movable_square;

    // Slider, Knights and King moves can only go to enemy or empty squares.
    if constexpr (mt == MoveGenType::ALL)
        movable_square = opp_empty;
    else if constexpr (mt == MoveGenType::CAPTURE)
        movable_square = occ_opp;
    else  // QUIET moves
        movable_square = ~occ_all;

    if (pieces & PieceGenType::KING) {
        Bitboard seen = seenSquares<~c>(board, opp_empty);

        whileBitboardAdd(movelist, Bitboard::fromSquare(king_sq),
                         [&](Square sq) { return generateKingMoves(sq, seen, movable_square); });

        if (mt != MoveGenType::CAPTURE && checks == 0) {
            Bitboard moves_bb = generateCastleMoves<c>(board, king_sq, seen, pin_hv);

            while (moves_bb) {
                Square to = moves_bb.pop();
                movelist.add(Move::make<Move::CASTLING>(king_sq, to));
            }
        }
    }

    // Early return for double check as described earlier
    if (checks == 2) return;

    // Moves have to be on the checkmask
    movable_square &= checkmask;

    // Add the moves to the movelist.
    if (pieces & PieceGenType::PAWN) {
        generatePawnMoves<c, mt>(board, movelist, pin_d, pin_hv, checkmask, occ_opp);
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

template <Color::underlying c>
inline bool movegen::isEpSquareValid(const Board &board, Square ep) {
    const auto stm = board.sideToMove();

    Bitboard occ_us  = board.us(stm);
    Bitboard occ_opp = board.us(~stm);
    auto king_sq     = board.kingSq(stm);

    const auto [checkmask, checks] = movegen::checkMask<c>(board, king_sq);
    const auto pin_hv              = movegen::pinMask<c, PieceType::ROOK>(board, king_sq, occ_opp, occ_us);
    const auto pin_d               = movegen::pinMask<c, PieceType::BISHOP>(board, king_sq, occ_opp, occ_us);

    const auto pawns    = board.pieces(PieceType::PAWN, stm);
    const auto pawns_lr = pawns & ~pin_hv;
    const auto m        = movegen::generateEPMove(board, checkmask, pin_d, pawns_lr, ep, stm);
    bool found          = false;

    for (const auto &move : m) {
        if (move != Move::NO_MOVE) {
            found = true;
            break;
        }
    }

    return found;
}

[[nodiscard]] inline Bitboard movegen::between(Square sq1, Square sq2) noexcept {
    return SQUARES_BETWEEN_BB[sq1.index()][sq2.index()];
}

inline const std::array<std::array<Bitboard, 64>, 64> movegen::SQUARES_BETWEEN_BB = [] {
    attacks::initAttacks();
    return movegen::init_squares_between();
}();

}  // namespace chess
