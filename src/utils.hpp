#pragma once

#include <vector>
#include <string_view>

#include "color.hpp"
#include "coords.hpp"

namespace chess {
namespace utils {
/// @brief Split a string into a vector of strings, using a delimiter.
/// @param string
/// @param delimiter
/// @return
[[nodiscard]] inline std::vector<std::string_view> splitString(std::string_view string,
                                                               const char &delimiter) {
    std::vector<std::string_view> result;
    size_t start = 0;
    size_t end   = string.find(delimiter);

    while (end != std::string_view::npos) {
        result.push_back(string.substr(start, end - start));
        start = end + 1;
        end   = string.find(delimiter, start);
    }

    // Add the last chunk (or the only chunk if there are no delimiters)
    result.push_back(string.substr(start));

    return result;
}

[[nodiscard]] constexpr Square relativeSquare(Color c, Square s) {
    return Square(s ^ (static_cast<int>(c.internal()) * 56));
}

/// @brief Checks if two squares have the same color. I.e light or dark.
/// @param sq1
/// @param sq2
/// @return
[[nodiscard]] constexpr bool sameColor(Square sq1, Square sq2) {
    return ((9 * static_cast<int>((sq1 ^ sq2).internal())) & 8) == 0;
}

/// @brief Checks if a square is on the back rank of a color.
/// @param sq
/// @param color
/// @return
[[nodiscard]] constexpr bool ourBackRank(Square sq, Color color) {
    if (color == Color::WHITE)
        return sq.rank() == Rank::RANK_1;
    else
        return sq.rank() == Rank::RANK_8;
}

/// @brief
/// @param r
/// @param f
/// @return
[[nodiscard]] constexpr int validSq(Rank r, File f) {
    return r >= Rank::RANK_1 && r <= Rank::RANK_8 && f >= File::FILE_A && f <= File::FILE_H;
}

}  // namespace utils

}  // namespace chess
