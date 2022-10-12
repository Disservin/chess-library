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
Square from(Move move) return 

/// @brief return the to square for a move
Square to(Move move)

/// @brief return the piecetype of the moved piece
PieceType piece(Move move)

/// @brief in case the move is a promotion it will return true
/// and piece(move) will return the promotion piece type
bool promoted(Move move)

/// @brief makes a move
Move make(PieceType piece = NONETYPE, Square source = NO_SQ, Square target = NO_SQ, bool promoted = false)

/// @brief make a move
template <PieceType piece, bool promoted> Move make(Square source = NO_SQ, Square target = NO_SQ)

/// @brief print the uci representation of a move
std::string uciRep(Move move)
```

Intrinsic functions
```cpp
Square bsf(U64 mask)
Square bsr(U64 mask)
Square popcount(U64 mask)
Square poplsb(U64 &mask)
```

Square related functions
```cpp
uint8_t square_file(Square sq)

uint8_t square_rank(Square sq)

uint8_t square_distance(Square a, Square b)

uint8_t manhatten_distance(Square sq1, Square sq2)

Color get_square_color(Square square)

```

Piece specific functions

```cpp
PieceType type_of_piece(Piece piece)
Piece makePiece(PieceType type, Color c)
```

Helper functions
```cpp

void printBitboard(U64 bb)
```

Attack move functions

```cpp

uint64_t PawnAttacks(Square sq, Color c)

uint64_t KnightAttacks(Square sq)

uint64_t BishopAttacks(Square sq, uint64_t occupied)

uint64_t RookAttacks(Square sq, uint64_t occupied)

uint64_t QueenAttacks(Square sq, uint64_t occupied)

uint64_t KingAttacks(Square sq)
```

Board class functions

```cpp
Board::Board(std::string fen)

void Board::applyFen(std::string fen, bool updateAcc);
std::string Board::getFen();

void Board::print();

Piece Board::pieceAtBB(Square sq)
Piece Board::pieceAtB(Square sq)

bool Board::isRepetition(int draw);

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


void Board::makeMove(Move move);

void Board::unmakeMove(Move move);

void Board::makeNullMove();

void Board::unmakeNullMove();

```

