## Overview:
A chess library for C++.<br>
Reasonably fast move generation for all purposes.<br>
Perft(6) `nodes: 119060324 nps 282133469`
Stockfish has `Nodes/second    : 247526661` although its also updating its NNUE during that.

Sample program of perft

```c++
#include "chess.hpp"

using namespace Chess;

uint64_t nodes;

uint64_t perft(Board &board, int depth)
{
    Movelist moves;
    Movegen::legalmoves<ALL>(board, moves);

    if (depth == 1)
    {
        return (int)moves.size;
    }
    uint64_t nodes = 0;
    for (int i = 0; i < (int)moves.size; i++)
    {
        Move move = moves[i].move;
        board.makeMove(move);
        nodes += perft(board, depth - 1);
        board.unmakeMove(move);
    }
    return nodes;
}

int main() {
    Board board = Board();
    auto t1 = std::chrono::high_resolution_clock::now();
    uint64_t n = perft(board, 6);
    auto t2 = std::chrono::high_resolution_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
    
    std::cout << "nodes " << n << " nps " << (n * 1000) / (ms + 1) << std::endl;
    return 0;
}
```

Move specific functions
```cpp

/// @brief return the from square for a move
Square from(Move move);

/// @brief return the to square for a move
Square to(Move move);

/// @brief return the piecetype of the moved piece
PieceType piece(Move move);

/// @brief in case the move is a promotion it will return true
/// and piece(move) will return the promotion piece type
bool promoted(Move move);

/// @brief makes a move
Move make(PieceType piece = NONETYPE, Square source = NO_SQ, Square target = NO_SQ, bool promoted = false);

/// @brief make a move
template <PieceType piece, bool promoted> Move make(Square source = NO_SQ, Square target = NO_SQ);

/// @brief print the uci representation of a move
std::string uciRep(Move move);

/// @brief requires the move to be 100% legal
void Board::makeMove(Move move);

/// @brief requires the move to be 100% legal
void Board::unmakeMove(Move move);

/// @brief make a null move
void Board::makeNullMove();

/// @brief make a null move
void Board::unmakeNullMove();

/// @brief generate ALL/CAPTURE/QUIET moves 
template <Movetype mt> void legalmoves(Board &board, Movelist &movelist);


```

Intrinsic functions
```cpp
/// @brief return the least significant bit
Square bsf(U64 mask);

/// @brief return the most signficicant bit
Square bsr(U64 mask);

/// @brief return the number of set bits
uint8_t popcount(U64 mask);

/// @brief return the least significant bit and remove it
Square poplsb(U64 &mask);
```

Square related functions
```cpp
/// @brief return the file a = 0, b = 1, c = 2, d = 3, e = 4, f = 5, g = 6, h = 7
uint8_t square_file(Square sq);

/// @brief return the rank, rank 1 is 0
uint8_t square_rank(Square sq);

/// @brief square distance between two squares
uint8_t square_distance(Square a, Square b);

/// @brief manhatten distance
uint8_t manhatten_distance(Square sq1, Square sq2);

/// @brief color of the square
Color get_square_color(Square square);

```

Piece specific functions

```cpp
/// @brief return the PieceType of a piece
PieceType type_of_piece(Piece piece);

/// @brief make a Piece 
Piece makePiece(PieceType type, Color c);
```

Helper functions
```cpp
/// @brief print any bitboard
void printBitboard(U64 bb);
```

Attack move functions

```cpp

uint64_t PawnAttacks(Square sq, Color c);

uint64_t KnightAttacks(Square sq);

/// @brief occupied = occupancy of both sides
uint64_t BishopAttacks(Square sq, uint64_t occupied);

/// @brief occupied = occupancy of both sides
uint64_t RookAttacks(Square sq, uint64_t occupied);

/// @brief occupied = occupancy of both sides
uint64_t QueenAttacks(Square sq, uint64_t occupied);

uint64_t KingAttacks(Square sq);
```

Board class functions

```cpp
Board::Board(std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

void Board::applyFen(std::string fen);
std::string Board::getFen();

void Board::print();

/// @brief uses bitboards to test what piece is on the square
Piece Board::pieceAtBB(Square sq);

/// @brief uses an array lookup to fetch the piece
Piece Board::pieceAtB(Square sq);

/// @brief detects if the current board is a repetition
bool Board::isRepetition(int draw = 3);

bool Board::nonPawnMat(Color c);
Square Board::KingSQ(Color c);

U64 Board::Pawns(Color c);
U64 Board::Knights(Color c);
U64 Board::Bishops(Color c);
U64 Board::Rooks(Color c);
U64 Board::Queens(Color c);
U64 Board::Kings(Color c);

U64 Board::Empty();
U64 Board::EnemyEmpty(Color c);
U64 Board::Us(Color c);
U64 Board::Enemy(Color c);
U64 Board::All();

bool Board::isSquareAttacked(Color c, Square sq);
```
