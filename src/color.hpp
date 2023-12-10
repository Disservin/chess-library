#pragma once

#include <cstdint>
#include <ostream>
#include <string>
#include <cassert>

namespace chess {

class Color {
   public:
    enum class underlying : std::int8_t { WHITE = 0, BLACK = 1, NO_COLOR = -1 };

    Color() = default;
    constexpr Color(underlying c) : color(c) {}
    constexpr Color(std::int8_t c) : color(static_cast<underlying>(c)) {
        assert(c == 0 || c == 1 || c == -1);
    }
    constexpr Color(std::string_view str) {
        if (str == "w") {
            color = underlying::WHITE;
        } else if (str == "b") {
            color = underlying::BLACK;
        } else {
            color = underlying::NO_COLOR;
        }
    }

    constexpr underlying internal() const { return color; }

    constexpr bool operator==(const Color& rhs) const { return color == rhs.color; }
    constexpr bool operator!=(const Color& rhs) const { return color != rhs.color; }

    constexpr Color operator~() const {
        switch (color) {
            case underlying::WHITE:
                return Color(underlying::BLACK);
            case underlying::BLACK:
                return Color(underlying::WHITE);
            default:
                return Color(underlying::NO_COLOR);
        }
    }

    constexpr operator std::string() const {
        switch (color) {
            case underlying::WHITE:
                return "w";
            case underlying::BLACK:
                return "b";
            default:
                return "NO_COLOR";
        }
    }

    friend std::ostream& operator<<(std::ostream& os, const Color& color);

    static constexpr underlying WHITE    = underlying::WHITE;
    static constexpr underlying BLACK    = underlying::BLACK;
    static constexpr underlying NO_COLOR = underlying::NO_COLOR;

   private:
    underlying color;
};  // namespace chess

std::ostream& operator<<(std::ostream& os, const Color& color) {
    return os << static_cast<std::string>(color);
}

}  // namespace chess
