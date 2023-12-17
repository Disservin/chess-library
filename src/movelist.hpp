#pragma once

#include <cassert>
#include <cstdint>

#include "move.hpp"
#include "constants.hpp"

namespace chess {
class Movelist {
   public:
    /// @brief Add a move to the end of the movelist.
    /// @param move
    constexpr void add(Move move) noexcept {
        assert(size_ < constants::MAX_MOVES);
        moves_[size_++] = move;
    }

    /// @brief Checks if a move is in the movelist, returns the index of the move if it is found,
    /// otherwise -1.
    /// @param move
    /// @return
    [[nodiscard]] constexpr int find(Move move) const noexcept {
        for (int i = 0; i < size_; ++i) {
            if (moves_[i] == move) {
                return i;
            }
        }

        return -1;
    }

    /// @brief Return the number of moves in the movelist.
    /// @return
    [[nodiscard]] constexpr int size() const noexcept { return size_; }

    /// @brief Checks if the movelist is empty.
    /// @return
    [[nodiscard]] constexpr bool empty() const noexcept { return size_ == 0; }

    /// @brief Clears the movelist.
    constexpr void clear() noexcept { size_ = 0; }

    [[nodiscard]] constexpr Move operator[](int index) const noexcept { return moves_[index]; }
    [[nodiscard]] constexpr Move &operator[](int index) noexcept { return moves_[index]; }

    using iterator       = Move *;
    using const_iterator = const Move *;

    constexpr iterator begin() { return moves_; }
    constexpr iterator end() { return moves_ + size_; }

    [[nodiscard]] constexpr const_iterator begin() const noexcept { return moves_; }
    [[nodiscard]] constexpr const_iterator end() const noexcept { return moves_ + size_; }

   private:
    Move moves_[constants::MAX_MOVES];
    int size_ = 0;
};
}  // namespace chess
