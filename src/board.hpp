#pragma once

#include <cstdint>
#include <array>

#include "coords.hpp"
#include "piece.hpp"
#include "color.hpp"

namespace chess {

class Board {
    using U64 = std::uint64_t;

    class CastlingRights {
       public:
        enum class Side : uint8_t { KING_SIDE, QUEEN_SIDE };

        void setCastlingRight(Color &&color, Side &castle, File &&rook_file) {
            rooks[static_cast<int>(color.internal())][static_cast<int>(castle)] = rook_file;
        }

        void clear() {
            rooks[0].fill(File::NO_FILE);
            rooks[1].fill(File::NO_FILE);
        }

        void clear(Color &&color, Side &castle) {
            rooks[static_cast<int>(color.internal())][static_cast<int>(castle)] = File::NO_FILE;
        }

        void clear(Color &&color) { rooks[static_cast<int>(color.internal())].fill(File::NO_FILE); }

        bool has(Color &&color, Side &&castle) const {
            return rooks[static_cast<int>(color.internal())][static_cast<int>(castle)] !=
                   File::NO_FILE;
        }

        bool has(Color &&color) const {
            return rooks[static_cast<int>(color.internal())][static_cast<int>(Side::KING_SIDE)] !=
                       File::NO_FILE ||
                   rooks[static_cast<int>(color.internal())][static_cast<int>(Side::QUEEN_SIDE)] !=
                       File::NO_FILE;
        }

        File getRookFile(Color &&color, Side &castle) const {
            return rooks[static_cast<int>(color.internal())][static_cast<int>(castle)];
        }

        int hashIndex() const {
            return has(Color::WHITE, Side::KING_SIDE) + 2 * has(Color::WHITE, Side::QUEEN_SIDE) +
                   4 * has(Color::BLACK, Side::KING_SIDE) + 8 * has(Color::BLACK, Side::QUEEN_SIDE);
        }

       private:
        // [color][side]
        std::array<std::array<File, 2>, 2> rooks;
    };

    struct State {
        CastlingRights castling;
        U64 hash;
        Square enpassant;
        uint8_t half_moves;
        Piece captured_piece;

        State(const U64 &hash, const CastlingRights &castling, const Square &enpassant,
              const uint8_t &half_moves, const Piece &captured_piece)
            : hash(hash),
              castling(castling),
              enpassant(enpassant),
              half_moves(half_moves),
              captured_piece(captured_piece) {}
    };

   public:
   private:
};
}  // namespace  chess
