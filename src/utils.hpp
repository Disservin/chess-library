#pragma once

#include <string_view>
#include <vector>

namespace chess {
namespace utils {
/// @brief Split a string into a vector of strings, using a delimiter.
/// @param string
/// @param delimiter
/// @return
[[nodiscard]] inline std::vector<std::string_view> splitString(std::string_view string, const char &delimiter) {
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

constexpr char tolower(char c) { return (c >= 'A' && c <= 'Z') ? (c - 'A' + 'a') : c; }

}  // namespace utils

}  // namespace chess
