#include "board.hpp"

int main(int argc, char const *argv[]) {
    chess::Board board;

    board.setFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");

    return 0;
}
