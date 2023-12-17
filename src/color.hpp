#pragma once

#include <cstdint>
#include <ostream>
#include <string>
#include <cassert>

namespace chess {

class Color {
   public:
    enum class underlying : std::int8_t { WHITE = 0, BLACK = 1, NONE = -1 };

    constexpr Color() : color(underlying::NONE) {}
    constexpr Color(underlying c) : color(c) {
        assert(c == underlying::WHITE || c == underlying::BLACK || c == underlying::NONE);
    }
    constexpr Color(int c) : color(static_cast<underlying>(c)) { assert(c == 0 || c == 1 || c == -1); }
    constexpr Color(std::string_view str) : color(underlying::NONE) {
        if (str == "w") {
            color = underlying::WHITE;
        } else if (str == "b") {
            color = underlying::BLACK;
        }
    }

    constexpr Color operator~() const noexcept { return static_cast<Color>(static_cast<uint8_t>(color) ^ 1); }

    explicit operator std::string() const {
        switch (color) {
            case underlying::WHITE:
                return "w";
            case underlying::BLACK:
                return "b";
            default:
                return "NONE";
        }
    }

    constexpr bool operator==(const Color& rhs) const noexcept { return color == rhs.color; }
    constexpr bool operator!=(const Color& rhs) const noexcept { return color != rhs.color; }

    constexpr operator int() const noexcept { return static_cast<int>(color); }

    [[nodiscard]] constexpr underlying internal() const noexcept { return color; }

    friend std::ostream& operator<<(std::ostream& os, const Color& color);

    static constexpr underlying WHITE = underlying::WHITE;
    static constexpr underlying BLACK = underlying::BLACK;
    static constexpr underlying NONE  = underlying::NONE;

   private:
    underlying color;
};  // namespace chess

inline std::ostream& operator<<(std::ostream& os, const Color& color) { return os << static_cast<std::string>(color); }

constexpr Color::underlying operator~(Color::underlying color) {
    switch (color) {
        case Color::underlying::WHITE:
            return Color::underlying::BLACK;
        case Color::underlying::BLACK:
            return Color::underlying::WHITE;
        default:
            return Color::underlying::NONE;
    }
}

}  // namespace chess
