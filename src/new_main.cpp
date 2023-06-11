#include "chess.hpp"

using namespace chess;

U64 perft(Board &board, int depth) {
    if (depth == 0) {
        return 1;
    }

    Movelist moves;
    movegen::legalmoves(moves, board);

    if (depth == 1) {
        return moves.size();
    }

    U64 nodes = 0;
    for (int i = 0; i < moves.size(); i++) {
        board.makeMove(moves[i]);

        nodes += perft(board, depth - 1);

        board.unmakeMove(moves[i]);
    }

    return nodes;
}

int main(int argc, char const *argv[]) {
    Board board;
    std::cout << board << std::endl;

    for (size_t i = 0; i < 7; i++) {
        std::cout << perft(board, i) << std::endl;
    }

    std::cout << board.isAttacked(Square::SQ_E4, Color::WHITE) << std::endl;

    auto move = uci::parseSan(board, "e4");
    std::cout << uci::moveToUci(move, false) << std::endl;
    std::cout << uci::moveToLan(board, move) << std::endl;
    std::cout << uci::moveToSan(board, move) << std::endl;

    board.makeMove(uci::parseSan(board, "e4"));
    if (board.hash() != 0x823c9b50fd114196) {
        std::cout << "Hash error" << std::endl;
        std::cout << board << std::endl;
    }

    board.makeMove(uci::parseSan(board, "d5"));
    if (board.hash() != 0x0756b94461c50fb0) {
        std::cout << "Hash error" << std::endl;
        std::cout << board << std::endl;
    }

    board.makeMove(uci::parseSan(board, "e4e5"));
    if (board.hash() != 0x662fafb965db29d4) {
        std::cout << "Hash error" << std::endl;
        std::cout << board << std::endl;
    }

    board.makeMove(uci::parseSan(board, "f7f5"));
    if (board.hash() != 0x22a48b5a8e47ff78) {
        std::cout << "Hash error" << std::endl;
        std::cout << board << std::endl;
    }

    board.makeMove(uci::parseSan(board, "Ke2"));
    if (board.hash() != 0x652a607ca3f242c1) {
        std::cout << "Hash error" << std::endl;
        std::cout << board << std::endl;
    }

    board.makeMove(uci::parseSan(board, "Kf7"));
    if (board.hash() != 0x00fdd303c946bdd9) {
        std::cout << "Hash error" << std::endl;
        std::cout << board << std::endl;
    }

    board = Board();

    board.makeMove(uci::parseSan(board, "a2a4"));
    board.makeMove(uci::parseSan(board, "b7b5"));
    board.makeMove(uci::parseSan(board, "h2h4"));
    board.makeMove(uci::parseSan(board, "b5b4"));
    board.makeMove(uci::parseSan(board, "c2c4"));
    if (board.hash() != 0x3c8123ea7b067637) {
        std::cout << "Hash error" << std::endl;
        std::cout << board << std::endl;
    }

    board.makeMove(uci::uciToMove(board, "b4c3"));
    board.makeMove(uci::parseSan(board, "Ra3"));
    if (board.hash() != 0x5c3f9b829b279560) {
        std::cout << "Hash error" << std::endl;
        std::cout << board << std::endl;
    }

    return 0;
}
