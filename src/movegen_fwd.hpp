#pragma once

#include <array>
#include <cstdint>
#include <utility>

#include "movelist.hpp"

namespace chess {
enum PieceGenType {
    PAWN   = 1,
    KNIGHT = 2,
    BISHOP = 4,
    ROOK   = 8,
    QUEEN  = 16,
    KING   = 32,
};

class Board;

class movegen {
   public:
    enum class MoveGenType : std::uint8_t { ALL, CAPTURE, QUIET };

    /**
     * @brief Generates all legal moves for a position.
     * @tparam mt
     * @param movelist
     * @param board
     * @param pieces
     */
    template <MoveGenType mt = MoveGenType::ALL>
    void static legalmoves(Movelist &movelist, const Board &board,
                           int pieces = PieceGenType::PAWN | PieceGenType::KNIGHT | PieceGenType::BISHOP |
                                        PieceGenType::ROOK | PieceGenType::QUEEN | PieceGenType::KING);

   private:
    static auto init_squares_between();
    static const std::array<std::array<Bitboard, 64>, 64> SQUARES_BETWEEN_BB;

    // Generate the checkmask. Returns a bitboard where the attacker path between the king and enemy piece is set.
    template <Color::underlying c>
    [[nodiscard]] static std::pair<Bitboard, int> checkMask(const Board &board, Square sq);

    // Generate the pin mask for horizontal and vertical pins. Returns a bitboard where the ray between the king and the
    // pinner is set.
    template <Color::underlying c>
    [[nodiscard]] static Bitboard pinMaskRooks(const Board &board, Square sq, Bitboard occ_enemy, Bitboard occ_us);

    // Generate the pin mask for diagonal pins. Returns a bitboard where the ray between the king and the pinner is set.
    template <Color::underlying c>
    [[nodiscard]] static Bitboard pinMaskBishops(const Board &board, Square sq, Bitboard occ_enemy, Bitboard occ_us);

    // Returns the squares that are attacked by the enemy
    template <Color::underlying c>
    [[nodiscard]] static Bitboard seenSquares(const Board &board, Bitboard enemy_empty);

    // Generate pawn moves.
    template <Color::underlying c, MoveGenType mt>
    static void generatePawnMoves(const Board &board, Movelist &moves, Bitboard pin_d, Bitboard pin_hv,
                                  Bitboard checkmask, Bitboard occ_enemy);

    [[nodiscard]] static std::array<Move, 2> generateEPMove(const Board &board, Bitboard checkmask, Bitboard pin_d,
                                                            Bitboard pawns_lr, Square ep, Color c);

    [[nodiscard]] static Bitboard generateKnightMoves(Square sq);

    [[nodiscard]] static Bitboard generateBishopMoves(Square sq, Bitboard pin_d, Bitboard occ_all);

    [[nodiscard]] static Bitboard generateRookMoves(Square sq, Bitboard pin_hv, Bitboard occ_all);

    [[nodiscard]] static Bitboard generateQueenMoves(Square sq, Bitboard pin_d, Bitboard pin_hv, Bitboard occ_all);

    [[nodiscard]] static Bitboard generateKingMoves(Square sq, Bitboard seen, Bitboard movable_square);

    template <Color::underlying c>
    [[nodiscard]] static Bitboard generateCastleMoves(const Board &board, Square sq, Bitboard seen, Bitboard pinHV);

    template <typename T>
    static void whileBitboardAdd(Movelist &movelist, Bitboard mask, T func);

    template <Color::underlying c, MoveGenType mt>
    static void legalmoves(Movelist &movelist, const Board &board, int pieces);

    template <Color::underlying c>
    static bool isEpSquareValid(const Board &board, Square ep);

    [[nodiscard]] static Bitboard between(Square sq1, Square sq2) noexcept {
        return SQUARES_BETWEEN_BB[sq1.index()][sq2.index()];
    }

    friend class Board;
};

}  // namespace chess
