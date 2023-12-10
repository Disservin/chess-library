#pragma once

#include <cstdint>

#include "coords.hpp"
#include "piece.hpp"

namespace chess {
class Move {
   public:
    Move() = default;
    constexpr Move(std::uint16_t move) : move_(move), score_(0) {}

    /// @brief Creates a move from a source and target square.
    /// pt is the promotion piece, when you want to create a promotion move you must also
    /// pass PROMOTION as the MoveType template parameter.
    /// @tparam MoveType
    /// @param source
    /// @param target
    /// @param pt
    /// @return
    template <std::uint16_t MoveType = 0>
    [[nodiscard]] static constexpr Move make(Square source, Square target,
                                             PieceType pt = PieceType::KNIGHT) {
        return Move(MoveType + ((std::uint16_t(pt) - std::uint16_t(PieceType::KNIGHT)) << 12) +
                    std::uint16_t(source << 6) + std::uint16_t(target));
    }

    /// @brief Get the source square of the move.
    /// @return
    [[nodiscard]] constexpr Square from() const { return static_cast<Square>((move_ >> 6) & 0x3F); }

    /// @brief Get the target square of the move.
    /// @return
    [[nodiscard]] constexpr Square to() const { return static_cast<Square>(move_ & 0x3F); }

    /// @brief Get the type of the move. Can be NORMAL, PROMOTION, ENPASSANT or CASTLING.
    /// @return
    [[nodiscard]] constexpr std::uint16_t typeOf() const {
        return static_cast<std::uint16_t>(move_ & (3 << 14));
    }

    /// @brief Get the promotion piece of the move, should only be used if typeOf() returns
    /// PROMOTION.
    /// @return
    [[nodiscard]] constexpr PieceType promotionType() const {
        return static_cast<PieceType>(((move_ >> 12) & 3) + static_cast<int>(PieceType::KNIGHT));
    }

    /// @brief Set the score for a move. Useful if you later want to sort the moves.
    /// @param score
    constexpr void setScore(std::int16_t score) { score_ = score; }

    [[nodiscard]] constexpr std::uint16_t move() const { return move_; }
    [[nodiscard]] constexpr std::int16_t score() const { return score_; }

    bool operator==(const Move &rhs) const { return move_ == rhs.move_; }
    bool operator!=(const Move &rhs) const { return move_ != rhs.move_; }

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

inline std::ostream &operator<<(std::ostream &os, const Move &move) {
    Square from_sq = move.from();
    Square to_sq   = move.to();

    os << from_sq << to_sq;
    if (move.typeOf() == Move::PROMOTION) {
        os << move.promotionType();
    }

    return os;
}
}  // namespace chess
