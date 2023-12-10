#include <cstdint>

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
}

class Board;

namespace chess::movegen {
enum class MoveGenType : std::uint8_t { ALL, CAPTURE, QUIET };

/// @brief Generates all legal moves for a position. The movelist will be
/// emptied before adding the moves.
/// @tparam mt
/// @param movelist
/// @param board
template <MoveGenType mt = MoveGenType::ALL>
void legalmoves(Movelist &movelist, const Board &board,
                int pieces = PieceGenType::PAWN | PieceGenType::KNIGHT | PieceGenType::BISHOP |
                             PieceGenType::ROOK | PieceGenType::QUEEN | PieceGenType::KING);

}  // namespace chess::movegen
