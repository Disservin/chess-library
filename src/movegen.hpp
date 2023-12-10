#pragma once

#include <array>
#include <cstdint>

#include "coords.hpp"
#include "attacks_fwd.hpp"

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
                    attacks::rook(Square(sq1), sqs) & attacks::rook(Square(sq2), sqs);
            else if (Square(sq1).diagonalOf() == Square(sq2).diagonalOf() ||
                     Square(sq1).antiDiagonalOf() == Square(sq2).antiDiagonalOf())
                squares_between_bb[sq1][sq2] =
                    attacks::bishop(Square(sq1), sqs) & attacks::bishop(Square(sq2), sqs);
        }
    }

    return squares_between_bb;
};

static const std::array<std::array<std::uint64_t, 64>, 64> SQUARES_BETWEEN_BB =
    init_squares_between();
}  // namespace chess::movegen
