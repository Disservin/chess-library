#pragma once

#include <cassert>
#include <cstdint>
#include <ostream>
#include <string>
#include <string_view>

namespace chess {

class Color {
   public:
    enum class underlying : std::int8_t { WHITE = 0, BLACK = 1, NONE = -1 };

    constexpr Color() : color(underlying::NONE) {}
    constexpr Color(underlying c) : color(c) { assert(isValid(static_cast<int>(c))); }
    constexpr Color(int c) : Color(static_cast<underlying>(c)) { assert(isValid(c)); }
    constexpr Color(std::string_view str)
        : color(str == "w"   ? underlying::WHITE
                : str == "b" ? underlying::BLACK
                             : underlying::NONE) {}

    /**
     * @brief Gets the long string representation of the color
     * @return "White" for WHITE, "Black" for BLACK
     * "None" for NONE
     */
    [[nodiscard]] std::string longStr() const {
        switch (color) {
            case underlying::WHITE:
                return "White";
            case underlying::BLACK:
                return "Black";
            default:
                return "None";
        }
    }

    constexpr Color operator~() const noexcept {
        assert(color != underlying::NONE);
        return Color(static_cast<underlying>(static_cast<int>(color) ^ 1));
    }

    constexpr bool operator==(const Color& rhs) const noexcept { return color == rhs.color; }
    constexpr bool operator!=(const Color& rhs) const noexcept { return color != rhs.color; }

    constexpr operator int() const noexcept { return static_cast<int>(color); }

    explicit operator std::string() const {
        return color == underlying::WHITE ? "w" : color == underlying::BLACK ? "b" : "NONE";
    }

    [[nodiscard]] constexpr underlying internal() const noexcept { return color; }

    friend std::ostream& operator<<(std::ostream& os, const Color& color) {
        return os << static_cast<std::string>(color);
    }

    static constexpr underlying WHITE = underlying::WHITE;
    static constexpr underlying BLACK = underlying::BLACK;
    static constexpr underlying NONE  = underlying::NONE;

   private:
    underlying color;

    static constexpr bool isValid(int c) { return c == 0 || c == 1 || c == -1; }
};

constexpr Color::underlying operator~(Color::underlying color) {
    return color == Color::underlying::WHITE   ? Color::underlying::BLACK
           : color == Color::underlying::BLACK ? Color::underlying::WHITE
                                               : Color::underlying::NONE;
}

}  // namespace chess
