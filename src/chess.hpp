#pragma once

#include <iostream>
#include <cstdint>
#include <vector>
#include <string>
#include <cstring>
#include <unordered_map>


namespace Chess {


/**********************************\
 ==================================

               Types 

 ==================================
\**********************************/


// enum type for mapping a color to int
enum Color : uint8_t {
    White, Black
};
    
// operator for swithing piece (e.g ~White = Black)
constexpr Color operator~(Color C) { 
    return Color(C ^ Black); 
}

// enum type for mapping a piece to int
enum Piece : uint8_t {
    WhitePawn, WhiteKnight, WhiteBishop, WhiteRook, WhiteQueen, WhiteKing, 
    BlackPawn, BlackKnight, BlackBishop, BlackRook, BlackQueen, BlackKing, 
    None
};

// enum type for mapping a square to int
enum Square : uint8_t {
    SQ_A1, SQ_B1, SQ_C1, SQ_D1, SQ_E1, SQ_F1, SQ_G1, SQ_H1,
    SQ_A2, SQ_B2, SQ_C2, SQ_D2, SQ_E2, SQ_F2, SQ_G2, SQ_H2,
    SQ_A3, SQ_B3, SQ_C3, SQ_D3, SQ_E3, SQ_F3, SQ_G3, SQ_H3,
    SQ_A4, SQ_B4, SQ_C4, SQ_D4, SQ_E4, SQ_F4, SQ_G4, SQ_H4,
    SQ_A5, SQ_B5, SQ_C5, SQ_D5, SQ_E5, SQ_F5, SQ_G5, SQ_H5,
    SQ_A6, SQ_B6, SQ_C6, SQ_D6, SQ_E6, SQ_F6, SQ_G6, SQ_H6,
    SQ_A7, SQ_B7, SQ_C7, SQ_D7, SQ_E7, SQ_F7, SQ_G7, SQ_H7,
    SQ_A8, SQ_B8, SQ_C8, SQ_D8, SQ_E8, SQ_F8, SQ_G8, SQ_H8,
    NO_SQ
};

// enums for Files
enum File : uint8_t {
    FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H
};

// enums for Ranks
enum Rank : uint8_t {
    RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8
};

// flags for makeMove function
enum Flag : uint8_t {
    allMoves, onlyCaptures
};

// castling rights masks
const uint8_t whiteKingSideCastling  = 1;
const uint8_t whiteQueenSideCastling = 2;
const uint8_t blackKingSideCastling  = 4;
const uint8_t blackQueenSideCastling = 8;


// default FEN string (start position)
const std::string defaultFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 ";

std::unordered_map<Piece, char> pieceToChar({
    { WhitePawn, 'P' },
    { WhiteKnight, 'N' },
    { WhiteBishop, 'B' },
    { WhiteRook, 'R' },
    { WhiteQueen, 'Q' },
    { WhiteKing, 'K' },
    { BlackPawn, 'p' },
    { BlackKnight, 'n' },
    { BlackBishop, 'b' },
    { BlackRook, 'r' },
    { BlackQueen, 'q' },
    { BlackKing, 'k' }
});

std::unordered_map<char, Piece> charToPiece({
    { 'P', WhitePawn },
    { 'N', WhiteKnight },
    { 'B', WhiteBishop },
    { 'R', WhiteRook },
    { 'Q', WhiteQueen },
    { 'K', WhiteKing },
    { 'p', BlackPawn },
    { 'n', BlackKnight },
    { 'b', BlackBishop },
    { 'r', BlackRook },
    { 'q', BlackQueen },
    { 'k', BlackKing }
});

const std::string squareToString[64] = {
    "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1",
    "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
    "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
    "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
    "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
    "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
    "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
    "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8",
};





/**********************************\
 ==================================

             Bitboards

 ==================================
\**********************************/


// define Bitboard as unsigned 64-bit integer
typedef uint64_t Bitboard;

//returns reversed bitboard (rotate 180 degrees)
Bitboard reverse(Bitboard bb) {
    bb = (bb & 0x5555555555555555) << 1 | ((bb >> 1) & 0x5555555555555555);
    bb = (bb & 0x3333333333333333) << 2 | ((bb >> 2) & 0x3333333333333333);
    bb = (bb & 0x0f0f0f0f0f0f0f0f) << 4 | ((bb >> 4) & 0x0f0f0f0f0f0f0f0f);
    bb = (bb & 0x00ff00ff00ff00ff) << 8 | ((bb >> 8) & 0x00ff00ff00ff00ff);

    return (bb << 48) | ((bb & 0xffff0000) << 16) | ((bb >> 16) & 0xffff0000) | (bb >> 48);
}


// Bit hack for getting the index of the least significant bit
// http://graphics.stanford.edu/~seander/bithacks.html
const int DEBRUIJN64[64] = {
    0, 47,  1, 56, 48, 27,  2, 60,
    57, 49, 41, 37, 28, 16,  3, 61,
    54, 58, 35, 52, 50, 42, 21, 44,
    38, 32, 29, 23, 17, 11,  4, 62,
    46, 55, 26, 59, 40, 36, 15, 53,
    34, 51, 20, 43, 31, 22, 10, 45,
    25, 39, 14, 33, 19, 30,  9, 24,
    13, 18,  8, 12,  7,  6,  5, 63
};

// returns index of least significant bit of given Bitboard
Square bsf(Bitboard bb) {
    return Square(DEBRUIJN64[0x03f79d71b4cb0a89 * (bb ^ (bb - 1)) >> 58]);
}

// returns index of LSB and removes that bit from given Bitboard
Square poplsb(Bitboard &bb) {
    Square lsb = bsf(bb);
    bb &= bb - 1;
    return lsb;
}

//Array containing bitboard for each square (1 << sq)
const Bitboard SQUARE_BB[64] = {
    0x1, 0x2, 0x4, 0x8,
    0x10, 0x20, 0x40, 0x80,
    0x100, 0x200, 0x400, 0x800,
    0x1000, 0x2000, 0x4000, 0x8000,
    0x10000, 0x20000, 0x40000, 0x80000,
    0x100000, 0x200000, 0x400000, 0x800000,
    0x1000000, 0x2000000, 0x4000000, 0x8000000,
    0x10000000, 0x20000000, 0x40000000, 0x80000000,
    0x100000000, 0x200000000, 0x400000000, 0x800000000,
    0x1000000000, 0x2000000000, 0x4000000000, 0x8000000000,
    0x10000000000, 0x20000000000, 0x40000000000, 0x80000000000,
    0x100000000000, 0x200000000000, 0x400000000000, 0x800000000000,
    0x1000000000000, 0x2000000000000, 0x4000000000000, 0x8000000000000,
    0x10000000000000, 0x20000000000000, 0x40000000000000, 0x80000000000000,
    0x100000000000000, 0x200000000000000, 0x400000000000000, 0x800000000000000,
    0x1000000000000000, 0x2000000000000000, 0x4000000000000000, 0x8000000000000000,
};

//file masks
const Bitboard MASK_FILE[8] = {
    0x101010101010101, 0x202020202020202, 0x404040404040404, 0x808080808080808,
    0x1010101010101010, 0x2020202020202020, 0x4040404040404040, 0x8080808080808080,
};

//rank masks
const Bitboard MASK_RANK[8] = {
    0xff, 0xff00, 0xff0000, 0xff000000,
    0xff00000000, 0xff0000000000, 0xff000000000000, 0xff00000000000000
};

//diagonal masks
const Bitboard MASK_DIAGONAL[15] = {
    0x80, 0x8040, 0x804020,
    0x80402010, 0x8040201008, 0x804020100804,
    0x80402010080402, 0x8040201008040201, 0x4020100804020100,
    0x2010080402010000, 0x1008040201000000, 0x804020100000000,
    0x402010000000000, 0x201000000000000, 0x100000000000000,
};

//anti-diagonal masks
const Bitboard MASK_ANTI_DIAGONAL[15] = {
    0x1, 0x102, 0x10204,
    0x1020408, 0x102040810, 0x10204081020,
    0x1020408102040, 0x102040810204080, 0x204081020408000,
    0x408102040800000, 0x810204080000000, 0x1020408000000000,
    0x2040800000000000, 0x4080000000000000, 0x8000000000000000
};


// sets bit at given square to 1
void setBit(Bitboard& bb, Square sq) {
    bb |= SQUARE_BB[sq];
}

// print given bitboard (for debugging purposes)
void printBitboard(Bitboard bb) {
    std::cout << "\n";
    for (int rank = 7; rank >= 0; rank--) {
        for (int file = 0; file < 8; file++) {
            Square sq = Square(rank * 8 + file);
            if (file == 0) std::cout << " " << rank + 1 << " ";
            std::cout << " " << (bb & (1ULL << sq) ? 1 : 0);
        }
        std::cout << "\n";
    }
    std::cout << "\n    a b c d e f g h\n\n";
}




/**********************************\
 ==================================

               Moves

 ==================================
\**********************************/


// class for representing a move
class Move {

private:
    // internal binary representation of a move
    uint32_t move;

public:
    // constructor for encoding a move
    inline Move(
        // main move info
        Square source,
        Square target,
        Piece piece,

        // move flags
        uint32_t promoted   = 0,
        uint32_t capture    = 0,
        uint32_t doublePush = 0,
        uint32_t enpassant  = 0,
        uint32_t castling   = 0
    ) {
        move = (uint32_t)source | (uint32_t)target << 6 | (uint32_t)piece << 12 |
            promoted << 16 | capture << 20 | doublePush << 21 | enpassant << 22 | castling << 23;

    }

    // extract source square
    inline Square source() {
        return Square(move & 0x3f);
    }

    // extract target square
    inline Square target() {
        return Square((move & 0xfc0) >> 6);
    }

    // extract moved piece
    inline Piece piece() {
        return Piece((move & 0xf000) >> 12);
    }

    // extract promoted piece
    inline Piece promoted() {
        return Piece((move & 0xf0000) >> 16);
    }

    // extract capture flag
    inline bool capture() {
        return (move & 0x100000) >> 20;
    }

    // extract double push flag
    inline bool doublePush() {
        return (move & 0x200000) >> 21;
    }

    // extract enpassant flag
    inline bool enpassant() {
        return (move & 0x400000) >> 22;
    }

    // extract castling flag
    inline bool castling() {
        return (move & 0x800000) >> 23;
    }
};

// define move list as a vector of moves
typedef std::vector<Move> Moves;




/**********************************\
 ==================================

               Board

 ==================================
\**********************************/

// struct for copying/storing previous states
struct State {
    Bitboard PiecesCopyBB[12];
    Piece boardCopy[64];
    Color sideToMoveCopy;
    Square enpassantCopy;
    uint8_t castlingRightsCopy;
};


class Board {
private:
    // array of piece bitboards
    Bitboard PiecesBB[12];

    // array of pieces on squares
    Piece board[64];

    // current side to move
    Color sideToMove;

    // current enpassant square
    Square enpassantSquare;

    // current castling rights
    uint8_t castlingRights;

    // store previous states
    State storeInfo;

public:
    // constructor for Board, take in a FEN string.
    // if no string is given, set board to default position
    Board(std::string FEN=defaultFEN);

    // returns a list of legal moves for current board state
    Moves generateLegalMoves();

    // prints the board 
    void print();

private:
    // sets the internal board representation to the 
    // FEN (Forsyth-Edwards Notation) string given
    void parseFEN(std::string FEN);

    // places a piece on a particular square
    void placePiece(Piece piece, Square sq);

    // removes a piece from a particular square
    void removePiece(Piece piece, Square sq);

    // returns a list of pseudo legal moves. Only used
    // in order to generate legal moves afterwards
    Moves generatePseudoLegalMoves();
};


// Board constructor that takes in FEN string.
// if no parameter given, set to default position
Board::Board(std::string FEN) {
    // reset board info
    memset(PiecesBB, 0ULL, sizeof(PiecesBB));
    memset(board, None, sizeof(board));

    enpassantSquare = NO_SQ;
    
    // parse FEN string
    parseFEN(FEN);
}

// parse FEN (Forsyth-Edwards Notation) string
void Board::parseFEN(std::string FEN) {
    // vector containing each section of FEN string
    std::vector<std::string> tokens;

    // general purpose string to handle current token
    std::string token;

    // split tokens by spaces
    for (int i = 0; i < FEN.size(); i++) {
        if (FEN[i] == ' ') {
            if (token.size() != 0) {
                tokens.push_back(token);
                token = "";
            }
            continue;
        }
        token += FEN[i];
    }

    // extract four sections from FEN string
    std::string pieces, color, castling, ep;

    if (tokens.size() >= 4) {
        pieces   = tokens[0];
        color    = tokens[1];
        castling = tokens[2];
        ep       = tokens[3];
    }
    else std::cout << "Invalid FEN string\n";

    // load pieces from FEN into internal board
    Square square = Square(56);
    for (int index = 0; index < pieces.size(); index++) {
        char curr = pieces[index];
        if (charToPiece.find(curr) != charToPiece.end()) {
            Piece piece = charToPiece[curr];
            placePiece(piece, square);
            square = Square(square + 1);
        }
        else if (curr == '/') square = Square(square - 16);
        else if (isdigit(curr)) square = Square(square + (curr - '0'));
    }

    // load the side to move for the position
    sideToMove = color == "w" ? White : Black;

    // set the enpassant square for the position
    if (ep != "-") {
        enpassantSquare = Square((ep[0] - '0') * 8 + (ep[1] - '0'));
    }

    // set castling rights for the position
    for (int i = 0; i < castling.size(); i++) {
        switch (castling[i]) {
        case 'K':
            castlingRights |= whiteKingSideCastling;
        case 'Q':
            castlingRights |= whiteQueenSideCastling;
        case 'k':
            castlingRights |= blackKingSideCastling;
        case 'q':
            castlingRights |= blackQueenSideCastling;
        }
    }
}

// place a piece on a particular square
void Board::placePiece(Piece piece, Square sq) {
    board[sq] = piece;
    PiecesBB[piece] |= SQUARE_BB[sq];
}

// remove a piece from a particular square
void Board::removePiece(Piece piece, Square sq) {
    PiecesBB[piece] &= ~SQUARE_BB[sq];
    board[sq] = None;
}

// print the current board state
void Board::print() {
    std::cout << "\n";
    for (int rank = 7; rank >= 0; rank--) {
        for (int file = 0; file < 8; file++) {
            Square sq = Square(rank * 8 + file);
            if (file == 0) std::cout << " " << rank + 1 << " ";
            Piece piece = board[sq];
            if (piece == None) std::cout << " .";
            else std::cout << " " << pieceToChar[piece];
        }
        std::cout << "\n";
    }
    std::cout << "\n    a b c d e f g h\n\n";
    std::cout << "   Side:    " << ((sideToMove == White) ? "White\n" : "Black\n");
    std::cout << "   Enpass:    " << ((enpassantSquare == NO_SQ) ? "NO_SQ" : squareToString[enpassantSquare]);
}




/**********************************\
 ==================================

         Move generation

 ==================================
\**********************************/



// funtion that returns a list of pseudo-legal moves
Moves Board::generatePseudoLegalMoves() {
    // init move list
    Moves moveList;

    return moveList;
}

// function that returns a list of legal moves
Moves Board::generateLegalMoves() {
    // init move list
    Moves moveList;

    return moveList;
}






}



