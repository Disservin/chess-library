
#include <iostream>
#include "color.hpp"
#include "board.hpp"
#include "attacks_fwd.hpp"
#include "attacks.hpp"
#include "movegen_fwd.hpp"
#include "movegen.hpp"

int main(int argc, char const *argv[]) {
    chess::Board board;

    board.setFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

std::cout << board << std::endl;

    chess::Movelist moves;
    chess::movegen::legalmoves(moves, board);

    std::cout << "Legal moves: " << moves.size() << std::endl;

    std::cout << board.us(chess::Color::WHITE) << std::endl;
    std::cout << board.us(~chess::Color::WHITE) << std::endl;
    std::cout << board.kingSq(chess::Color::WHITE) << std::endl;

    for (auto move : moves) {
        std::cout << move << std::endl;
    }

    return 0;
}
