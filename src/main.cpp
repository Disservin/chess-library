
#include <iostream>
#include "board.hpp"
#include "attacks_fwd.hpp"
#include "attacks.hpp"
#include "movegen_fwd.hpp"
#include "movegen.hpp"

int main(int argc, char const *argv[]) {
    std::cout << "Hello, World!" << std::endl;
    chess::Board board;
    std::cout << board << std::endl;

    board.setFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");

    std::cout << board << std::endl;

    chess::Movelist moves;
    chess::movegen::legalmoves(moves, board);

    for (auto move : moves) {
        std::cout << move << std::endl;
    }

    return 0;
}
