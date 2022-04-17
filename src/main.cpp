#include <iostream>

#include "chess.hpp"

using namespace Chess;

Board board = Board();

unsigned long long perft(int depth, int maxDepth) {
    if (depth == 0) {
        return 1;
    }

    unsigned long long count = 0;
    unsigned long long total = 0;
    Moves moveList = board.generatePseudoLegalMoves();
    for (int i = 0; i < (int)moveList.size(); i++) {
        Move move = moveList[i];
        board.makeMove(move);
        count += perft(depth - 1, maxDepth);
        board.unmakeMove();
        if (depth == maxDepth){
            std::cout << squareToString[move.source()] << squareToString[move.target()];
            std::cout << ": " << count << std::endl;
            total += count;
            count = 0;
        }
    }
    if (depth == maxDepth)
        std::cout << "Total: " << total << std::endl;
    return count;
}

int main() {
    // board.print();
    
    while (true){
        std::string input;
        std::getline(std::cin, input);
        if (input.find("position fen") != std::string::npos) {
            std::size_t start_index = input.find("fen");
            std::string fen = input.substr(start_index + 4);
            board.parseFEN(fen);
        }
        if (input.find("go perft") != std::string::npos){
            std::size_t start_index = input.find("perft");
            std::string depth_str = input.substr(start_index + 6);
            int depth = std::stoi(depth_str);
            perft(depth, depth);
        }
        if (input == "quit")
            break;
    }
    
}