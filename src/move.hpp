#pragma once

#include <cstdint>

#include "coords.hpp"
#include "piece.hpp"

namespace chess {

class Move {
   public:
    Move() = default;
    constexpr Move(std::uint16_t move) : move_(move), score_(0) {}

    /**
     * @brief Creates a move from a source and target square.
     * @tparam MoveType
     * @param source
     * @param target
     * @param pt leave this empty if it is not a promotion move, otherwise pass the piece type of the new piece.
     * @return
     */
    template <std::uint16_t MoveType = 0>
    [[nodiscard]] static constexpr Move make(Square source, Square target, PieceType pt = PieceType::KNIGHT) noexcept {
        assert(pt >= PieceType(PieceType::KNIGHT) && pt <= PieceType(PieceType::QUEEN));

        std::uint16_t bits_promotion = static_cast<std::uint16_t>(pt - PieceType(PieceType::KNIGHT));

        return Move(MoveType + (bits_promotion << 12) + (source.index() << 6) + target.index());
    }

    /**
     * @brief Get the source square of the move.
     * @return
     */
    [[nodiscard]] constexpr Square from() const noexcept { return static_cast<Square>((move_ >> 6) & 0x3F); }

    /**
     * @brief Get the target square of the move.
     * @return
     */
    [[nodiscard]] constexpr Square to() const noexcept { return static_cast<Square>(move_ & 0x3F); }

    /**
     * @brief Get the type of the move. Can be NORMAL, PROMOTION, ENPASSANT or CASTLING.
     * @return
     */
    [[nodiscard]] constexpr std::uint16_t typeOf() const noexcept {
        return static_cast<std::uint16_t>(move_ & (3 << 14));
    }

    /**
     * @brief Get the promotion piece of the move, should only be used if typeOf() returns PROMOTION.
     * @return
     */
    [[nodiscard]] constexpr PieceType promotionType() const noexcept {
        return static_cast<PieceType::underlying>(((move_ >> 12) & 3) + PieceType(PieceType::KNIGHT));
    }

    /**
     * @brief Set the score for a move. Useful if you later want to sort the moves.
     * @param score
     */
    constexpr void setScore(std::int16_t score) noexcept { score_ = score; }

    [[nodiscard]] constexpr std::uint16_t move() const noexcept { return move_; }
    [[nodiscard]] constexpr std::int16_t score() const noexcept { return score_; }

    constexpr bool operator==(const Move &rhs) const noexcept { return move_ == rhs.move_; }
    constexpr bool operator!=(const Move &rhs) const noexcept { return move_ != rhs.move_; }

    static constexpr std::uint16_t NO_MOVE   = 0;
    static constexpr std::uint16_t NULL_MOVE = 65;
    static constexpr std::uint16_t NORMAL    = 0;
    static constexpr std::uint16_t PROMOTION = 1 << 14;
    static constexpr std::uint16_t ENPASSANT = 2 << 14;
    static constexpr std::uint16_t CASTLING  = 3 << 14;

   private:
    std::uint16_t move_;
    std::int16_t score_;
};

}  // namespace chess
