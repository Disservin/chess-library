#pragma once

#include <array>
#include <cassert>
#include <cstddef>
#include <iterator>
#include <stdexcept>
#include <string>

#include "constants.hpp"
#include "move.hpp"

namespace chess {
class Movelist {
   public:
    using value_type      = Move;
    using size_type       = int;
    using difference_type = std::ptrdiff_t;
    using reference       = value_type&;
    using const_reference = const value_type&;
    using pointer         = value_type*;
    using const_pointer   = const value_type*;

    using iterator       = value_type*;
    using const_iterator = const value_type*;

    using reverse_iterator       = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    // Element access

    [[nodiscard]] constexpr reference at(size_type pos) {
        if (pos >= size_) {
            throw std::out_of_range("Movelist::at: pos (which is " + std::to_string(pos) + ") >= size (which is " +
                                    std::to_string(size_) + ")");
        }
        return moves_[pos];
    }

    [[nodiscard]] constexpr const_reference at(size_type pos) const {
        if (pos >= size_) {
            throw std::out_of_range("Movelist::at: pos (which is " + std::to_string(pos) + ") >= size (which is " +
                                    std::to_string(size_) + ")");
        }
        return moves_[pos];
    }

    [[nodiscard]] constexpr reference operator[](size_type pos) noexcept { return moves_[pos]; }
    [[nodiscard]] constexpr const_reference operator[](size_type pos) const noexcept { return moves_[pos]; }

    [[nodiscard]] constexpr reference front() noexcept { return moves_[0]; }
    [[nodiscard]] constexpr const_reference front() const noexcept { return moves_[0]; }

    [[nodiscard]] constexpr reference back() noexcept { return moves_[size_ - 1]; }
    [[nodiscard]] constexpr const_reference back() const noexcept { return moves_[size_ - 1]; }

    // Iterators

    [[nodiscard]] constexpr auto begin() noexcept { return moves_.begin(); }
    [[nodiscard]] constexpr auto begin() const noexcept { return moves_.begin(); }

    [[nodiscard]] constexpr auto end() noexcept { return moves_.begin() + size_; }
    [[nodiscard]] constexpr auto end() const noexcept { return moves_.begin() + size_; }

    [[nodiscard]] constexpr auto rbegin() noexcept {
        return moves_.rbegin() + (constants::MAX_MOVES - size_);
    }

    [[nodiscard]] constexpr auto rbegin() const noexcept {
        return moves_.rbegin() + (constants::MAX_MOVES - size_);
    }

    [[nodiscard]] constexpr auto rend() noexcept { return moves_.rbegin() + constants::MAX_MOVES; }
    [[nodiscard]] constexpr auto rend() const noexcept {
        return moves_.rbegin() + constants::MAX_MOVES;
    }

    // Capacity

    /// @brief Checks if the movelist is empty.
    /// @return
    [[nodiscard]] constexpr bool empty() const noexcept { return size_ == 0; }

    /// @brief Return the number of moves in the movelist.
    /// @return
    [[nodiscard]] constexpr size_type size() const noexcept { return size_; }

    // Modifiers

    /// @brief Clears the movelist.
    constexpr void clear() noexcept { size_ = 0; }

    /// @brief Add a move to the end of the movelist.
    /// @param move
    constexpr void add(const_reference move) noexcept {
        assert(size_ < constants::MAX_MOVES);
        moves_[size_++] = move;
    }

    /// @brief Add a move to the end of the movelist.
    /// @param move
    constexpr void add(value_type&& move) noexcept {
        assert(size_ < constants::MAX_MOVES);
        moves_[size_++] = move;
    }

    // Other

    /// @brief Checks if a move is in the movelist, returns the index of the move if it is found,
    /// otherwise -1.
    /// @param move
    /// @return
    [[nodiscard]] [[deprecated("Use std::find() instead.")]] constexpr size_type find(value_type move) const noexcept {
        for (size_type i = 0; i < size_; ++i) {
            if (moves_[i] == move) {
                return i;
            }
        }

        return -1;
    }

   private:
    std::array<value_type, constants::MAX_MOVES> moves_;
    size_type size_ = 0;
};
}  // namespace chess
