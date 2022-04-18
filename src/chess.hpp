#pragma once

#define Compiletime __forceinline constexpr

#include <iostream>
#include <cstdint>
#include <vector>
#include <string>
#include <cstring>
#include <unordered_map>
#include <bitset>
#include <cmath>
#include <algorithm>

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

enum PieceType : uint8_t {
    Pawn, Knight, Bishop, Rook, Queen, King
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

// operator for incrementing a square
inline Square& operator++(Square& s) { 
    return s = Square(int(s) + 1); 
}

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

// map a piece to its corresponding character
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

// map a character to its corresponding piece
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

// map a square to its string representation
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

//returns reversed bitboard (rotate 180 degrees)
inline Bitboard reverse(Bitboard bb) {
    bb = (bb & 0x5555555555555555) << 1 | ((bb >> 1) & 0x5555555555555555);
    bb = (bb & 0x3333333333333333) << 2 | ((bb >> 2) & 0x3333333333333333);
    bb = (bb & 0x0f0f0f0f0f0f0f0f) << 4 | ((bb >> 4) & 0x0f0f0f0f0f0f0f0f);
    bb = (bb & 0x00ff00ff00ff00ff) << 8 | ((bb >> 8) & 0x00ff00ff00ff00ff);

    return (bb << 48) | ((bb & 0xffff0000) << 16) | ((bb >> 16) & 0xffff0000) | (bb >> 48);
}

// Compiler specific functions, taken from Stockfish https://github.com/official-stockfish/Stockfish
#if defined(__GNUC__)  // GCC, Clang, ICC
#include <nmmintrin.h>
inline Square bsf(Bitboard b) {

  return Square(__builtin_ctzll(b));
}

inline Square bsr(Bitboard b) {

  return Square(63 ^ __builtin_clzll(b));
}

#elif defined(_MSC_VER)  // MSVC

#ifdef _WIN64  // MSVC, WIN64
#include <intrin.h>
inline Square bsf(Bitboard b) {

  unsigned long idx;
  _BitScanForward64(&idx, b);
  return (Square) idx;
}

inline Square bsr(Bitboard b) {

  unsigned long idx;
  _BitScanReverse64(&idx, b);
  return (Square) idx;
}

#else  // MSVC, WIN32
#include <intrin.h>
inline Square bsf(Bitboard b) {

  unsigned long idx;

  if (b & 0xffffffff) {
      _BitScanForward(&idx, int32_t(b));
      return Square(idx);
  } else {
      _BitScanForward(&idx, int32_t(b >> 32));
      return Square(idx + 32);
  }
}

inline Square bsr(Bitboard b) {

  unsigned long idx;

  if (b >> 32) {
      _BitScanReverse(&idx, int32_t(b >> 32));
      return Square(idx + 32);
  } else {
      _BitScanReverse(&idx, int32_t(b));
      return Square(idx);
  }
}

#endif

#else  // Compiler is neither GCC nor MSVC compatible

#error "Compiler not supported."

#endif

inline int popCount(Bitboard b){
#if defined(_MSC_VER) || defined(__INTEL_COMPILER)

  return (int)_mm_popcnt_u64(b);

#else // Assumed gcc or compatible compiler

  return __builtin_popcountll(b);

#endif
}

// returns index of LSB and removes that bit from given Bitboard
Square poplsb(Bitboard &bb) {
    Square lsb = bsf(bb);
    bb &= bb - 1;
    return lsb;
}

// sets bit at given square to 1
inline void setBit(Bitboard& bb, Square sq) {
    bb |= SQUARE_BB[sq];
}

inline bool isBitSet(Bitboard bb, Square sq) {
    return (bb & SQUARE_BB[sq]) ? true : false;
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
         Helper Functions
 ==================================
\**********************************/

// returns the rank of a given square
uint8_t rank_of(Square sq) {
    return sq >> 3;
}

// returns the file of a given square
uint8_t file_of(Square sq) {
    return sq & 7;
}

// returns diagonal of given square
uint8_t diagonal_of(Square sq) {
    return 7 + rank_of(sq) - file_of(sq);
}

// returns anti diagonal of given square
uint8_t anti_diagonal_of(Square sq) {
    return rank_of(sq) + file_of(sq);
}

// returns the piece type
PieceType piece_type(Piece p){
    return PieceType(p % 6);
}

template <Color c>
Piece makePiece(PieceType type) {
  return Piece(6 * c + type);
}

Color piece_color(Piece p){
    return Color(p / 6);
}

inline int squareDistance(Square a, Square b) {
    return std::max(std::abs(file_of(a) - file_of(b)), std::abs(rank_of(a) - rank_of(b)));
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
        Square source  = NO_SQ,
        Square target  = NO_SQ,
        Piece piece    = None,
        Piece promoted = None,

        // move flags
        uint32_t capture    = 0,
        uint32_t doublePush = 0,
        uint32_t enpassant  = 0,
        uint32_t castling   = 0
    ) {
        move = (uint32_t)source | (uint32_t)target << 6 | (uint32_t)piece << 12 |
        (uint32_t)promoted << 16 | capture << 20 | doublePush << 21 | enpassant << 22 | castling << 23;

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

    // returns move in UCI string format
    std::string toUci() {
        return squareToString[this->source()] + squareToString[this->target()];
    }
};

// move list
struct Moves {
    Move moves[256]{};
    uint8_t count{};

    void Add(Move move) {
        moves[count] = move;
        count++;
    }
};




// define move list as a vector of moves
//typedef std::vector<Move> Moves;

void printMove(Move move) {
    std::cout << "Move: " << squareToString[move.source()] << squareToString[move.target()] << " |";
    std::cout << " Piece: " << pieceToChar[move.piece()] << " |";
    std::cout << " Promoted: " << (move.promoted()==None ? "None" : std::string(1, pieceToChar[move.promoted()])) << " |";
    std::cout << " Capture: " << move.capture() << " |";
    std::cout << " Double Push: " << move.doublePush() << " |";
    std::cout << " Enpassant: " << move.enpassant() << " |";
    std::cout << " Castling: " << move.castling() << " |"; 
    std::cout << std::endl;
}


/**********************************\
 ==================================
               Board
 ==================================
\**********************************/

// struct for copying/storing previous states
struct State {
    Square   enpassantCopy;
    uint8_t  castlingRightsCopy;
    Piece  capturedPiece;
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
    State storeInfo[1024];

    uint16_t storeCount;

    // store hash key of previous states
    uint64_t gameHistory[1024];

    // Checkmask
    Bitboard checkMask = 18446744073709551615ULL;

    // Horizontal/Vertical Pinmask
    Bitboard pinMaskHV;

    // Diagonal Pinmask
    Bitboard pinMaskD;

    // Doublecheck
    uint8_t doubleCheck{};

    // Occupancy bitboard, done during generateMoves
    Bitboard occupancyWhite;
    Bitboard occupancyBlack;
    Bitboard occupancyAll;

    // lookup table for squares between two squares
    Bitboard SQUARES_BETWEEN_BB[64][64];

    void initializeLookupTables();

public:
    // constructor for Board, take in a FEN string.
    // if no string is given, set board to default position
    Board(std::string FEN=defaultFEN);

    // returns a list of legal moves for current board state
    template <Color c> Moves generateLegalMoves();

    // prints the entire board 
    void print();

    // functions for getting individual
    // piece bitboards
    template <Color c> Compiletime Bitboard Pawns();
    template <Color c> Compiletime Bitboard Knights();
    template <Color c> Compiletime Bitboard Bishops();
    template <Color c> Compiletime Bitboard Rooks();
    template <Color c> Compiletime Bitboard Queens();
    template <Color c> Compiletime Bitboard Kings();
    template <Color c> Compiletime Bitboard allPieces();
    template <Color c> Compiletime Bitboard Enemy();
    template <Color c> Compiletime Bitboard EnemyEmpty();
    template <Color c> Compiletime Bitboard PieceBB(PieceType type);

    template <Color c> Square KingSq();
    template <Color c> Bitboard doCheckmask(Square sq);
    template <Color c> void create_pins(Square sq);
    template <Color c> void init(Square sq);

    template <Color c> Bitboard LegalPawnMoves(Square sq);
    template <Color c> Bitboard LegalKnightMoves(Square sq);
    template <Color c> Bitboard LegalBishopMoves(Square sq);
    template <Color c> Bitboard LegalRookMoves(Square sq);
    template <Color c> Bitboard LegalQueenMoves(Square sq);
    template <Color c> Bitboard LegalKingMoves(Square sq);

    template <Color c> void makemove(Move& move);
    template <Color c> void unmakemove(Move& move);

    Piece getPiece(Square sq);

    // sets the internal board representation to the 
    // FEN (Forsyth-Edwards Notation) string given
    void parseFEN(std::string FEN);
private:

    // places a piece on a particular square
    void placePiece(Piece piece, Square sq);

    // removes a piece from a particular square
    void removePiece(Piece piece, Square sq);

    // functions for pawn, knight and king movegen
    template <Color c> inline Bitboard GetPawnPush(Square sq); 
    template <Color c> inline Bitboard GetPawnAttacks(Square square);
    inline Bitboard GetKnightAttacks(Square square); 
    inline Bitboard GetKingAttacks(Square square);

    // functions for sliders movegen
    inline Bitboard hyp_quint(Square square, Bitboard occ, Bitboard mask);
    inline Bitboard GetBishopAttacks(Square square, Bitboard occ); 
    inline Bitboard GetRookAttacks(Square square, Bitboard occ); 
    inline Bitboard GetQueenAttacks(Square square, Bitboard occ); 

public:
    // checks if a square is being attacked by the given side
    template <Color c> bool isSquareAttacked(Square sq);
};

// place a piece on a particular square
inline void Board::placePiece(Piece piece, Square sq) {
    PiecesBB[piece] |= SQUARE_BB[sq];
    board[sq] = piece;
}

// remove a piece from a particular square
inline void Board::removePiece(Piece piece, Square sq) {
    PiecesBB[piece] &= ~SQUARE_BB[sq];
    board[sq] = None;
}

// returns pawns bitboard for given color
template <Color c> 
Compiletime Bitboard Board::Pawns(){
    return PiecesBB[c * 6];
}

// returns knights bitboard for given color
template <Color c> 
Compiletime Bitboard Board::Knights(){
    return PiecesBB[c * 6 + Knight];
}

// returns bishops bitboard for given color
template <Color c> 
Compiletime Bitboard Board::Bishops(){
    return PiecesBB[c * 6 + Bishop];
}

// returns rooks bitboard for given color
template <Color c> 
Compiletime Bitboard Board::Rooks(){
    return PiecesBB[c * 6 + Rook];
}

// returns queens bitboard for given color
template <Color c> 
Compiletime Bitboard Board::Queens(){
    return PiecesBB[c * 6 + Queen];
}


// returns king bitboard for given color
template <Color c> 
Compiletime Bitboard Board::Kings(){
    return PiecesBB[c * 6 + King];
}

// returns bitboard containing all pieces of given color
template <Color c> 
Compiletime Bitboard Board::allPieces(){
    return Pawns<c>() | Knights<c>() | Bishops<c>() | Rooks<c>() | Queens<c>() | Kings<c>();
}

template <Color c> 
Compiletime Bitboard Board::Enemy(){
    if constexpr (c==White) return allPieces<Black>();
    return allPieces<White>();
}

template <Color c> 
Compiletime Bitboard Board::EnemyEmpty(){
    if constexpr (c==White) return ~allPieces<White>();
    return ~allPieces<Black>();
}

template <Color c> 
Compiletime Bitboard Board::PieceBB(PieceType type) {
    return PiecesBB[c * 6 + type];
}

template <Color c> 
inline Square Board::KingSq(){
    if constexpr (c==White) return bsf(Kings<White>());
    return bsf(Kings<Black>());
}

void Board::initializeLookupTables() {
    //initialize squares between table
    Bitboard sqs;
    for (Square sq1 = SQ_A1; sq1 <= SQ_H8; ++sq1) {
        for (Square sq2 = SQ_A1; sq2 <= SQ_H8; ++sq2) {
            sqs = SQUARE_BB[sq1] | SQUARE_BB[sq2];
            if (file_of(sq1) == file_of(sq2) || rank_of(sq1) == rank_of(sq2))
                SQUARES_BETWEEN_BB[sq1][sq2] =
                GetRookAttacks(sq1, sqs) & GetRookAttacks(sq2, sqs);
            else if (diagonal_of(sq1) == diagonal_of(sq2) || anti_diagonal_of(sq1) == anti_diagonal_of(sq2))
                SQUARES_BETWEEN_BB[sq1][sq2] =
                GetBishopAttacks(sq1, sqs) & GetBishopAttacks(sq2, sqs);
        }   
    }
}

// Board constructor that takes in FEN string.
// if no parameter given, set to default position
Board::Board(std::string FEN) {
    // init lookup tables used for movegen
    initializeLookupTables();

    // reset board info
    memset(PiecesBB, 0ULL, sizeof(PiecesBB));
    memset(board, None, sizeof(board));

    // reset enpassant square
    enpassantSquare = NO_SQ;

    // reset castling rights
    castlingRights = 0;
    
    // parse FEN string
    parseFEN(FEN);
}

// parse FEN (Forsyth-Edwards Notation) string
void Board::parseFEN(std::string FEN) {
    // reset board info
    memset(PiecesBB, 0ULL, sizeof(PiecesBB));
    memset(board, None, sizeof(board));

    // reset enpassant square
    enpassantSquare = NO_SQ;

    // reset castling rights
    castlingRights = 0;
    // vector containing each section of FEN string
    std::vector<std::string> tokens;

    // general purpose string to handle current token
    std::string token;

    // split tokens by spaces
    for (int i = 0; i < (int)FEN.size(); i++) {
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
    for (int index = 0; index < (int)pieces.size(); index++) {
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
        int rank = ((int)ep[1]) - 49;
        int file = ((int)ep[0]) - 97;
        enpassantSquare = Square(rank * 8 + file);
    }


    // set castling rights for the position
    for (int i = 0; i < (int)castling.size(); i++) {
        switch (castling[i]) {
        case 'K':
            castlingRights |= whiteKingSideCastling;
            break;
        case 'Q':
            castlingRights |= whiteQueenSideCastling;
            break;
        case 'k':
            castlingRights |= blackKingSideCastling;
            break;
        case 'q':
            castlingRights |= blackQueenSideCastling;
            break;
        }
    }
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

    std::cout << "   Castling:  ";
    std::cout << ((castlingRights & whiteKingSideCastling) ? "K" : "-");
    std::cout << ((castlingRights & whiteQueenSideCastling) ? "Q" : "-");
    std::cout << ((castlingRights & blackKingSideCastling) ? "k" : "-");
    std::cout << ((castlingRights & blackQueenSideCastling) ? "q" : "-");

    std::cout << "\n   Enpass:    " << ((enpassantSquare == NO_SQ) ? "NO_SQ" : squareToString[enpassantSquare]);
    std::cout << "\n";
}

/**********************************\
 ==================================
         Move generation
 ==================================
\**********************************/


// pre calculated lookup table for knight attacks
static constexpr Bitboard KNIGHT_ATTACKS_TABLE[64] = {
    0x0000000000020400, 0x0000000000050800, 0x00000000000A1100, 0x0000000000142200,
    0x0000000000284400, 0x0000000000508800, 0x0000000000A01000, 0x0000000000402000,
    0x0000000002040004, 0x0000000005080008, 0x000000000A110011, 0x0000000014220022,
    0x0000000028440044, 0x0000000050880088, 0x00000000A0100010, 0x0000000040200020,
    0x0000000204000402, 0x0000000508000805, 0x0000000A1100110A, 0x0000001422002214,
    0x0000002844004428, 0x0000005088008850, 0x000000A0100010A0, 0x0000004020002040,
    0x0000020400040200, 0x0000050800080500, 0x00000A1100110A00, 0x0000142200221400,
    0x0000284400442800, 0x0000508800885000, 0x0000A0100010A000, 0x0000402000204000,
    0x0002040004020000, 0x0005080008050000, 0x000A1100110A0000, 0x0014220022140000,
    0x0028440044280000, 0x0050880088500000, 0x00A0100010A00000, 0x0040200020400000,
    0x0204000402000000, 0x0508000805000000, 0x0A1100110A000000, 0x1422002214000000,
    0x2844004428000000, 0x5088008850000000, 0xA0100010A0000000, 0x4020002040000000,
    0x0400040200000000, 0x0800080500000000, 0x1100110A00000000, 0x2200221400000000,
    0x4400442800000000, 0x8800885000000000, 0x100010A000000000, 0x2000204000000000,
    0x0004020000000000, 0x0008050000000000, 0x00110A0000000000, 0x0022140000000000,
    0x0044280000000000, 0x0088500000000000, 0x0010A00000000000, 0x0020400000000000
};

// pre calculated lookup table for king attacks
static constexpr Bitboard KING_ATTACKS_TABLE[64] = {
    0x0000000000000302, 0x0000000000000705, 0x0000000000000E0A, 0x0000000000001C14,
    0x0000000000003828, 0x0000000000007050, 0x000000000000E0A0, 0x000000000000C040,
    0x0000000000030203, 0x0000000000070507, 0x00000000000E0A0E, 0x00000000001C141C,
    0x0000000000382838, 0x0000000000705070, 0x0000000000E0A0E0, 0x0000000000C040C0,
    0x0000000003020300, 0x0000000007050700, 0x000000000E0A0E00, 0x000000001C141C00,
    0x0000000038283800, 0x0000000070507000, 0x00000000E0A0E000, 0x00000000C040C000,
    0x0000000302030000, 0x0000000705070000, 0x0000000E0A0E0000, 0x0000001C141C0000,
    0x0000003828380000, 0x0000007050700000, 0x000000E0A0E00000, 0x000000C040C00000,
    0x0000030203000000, 0x0000070507000000, 0x00000E0A0E000000, 0x00001C141C000000,
    0x0000382838000000, 0x0000705070000000, 0x0000E0A0E0000000, 0x0000C040C0000000,
    0x0003020300000000, 0x0007050700000000, 0x000E0A0E00000000, 0x001C141C00000000,
    0x0038283800000000, 0x0070507000000000, 0x00E0A0E000000000, 0x00C040C000000000,
    0x0302030000000000, 0x0705070000000000, 0x0E0A0E0000000000, 0x1C141C0000000000,
    0x3828380000000000, 0x7050700000000000, 0xE0A0E00000000000, 0xC040C00000000000,
    0x0203000000000000, 0x0507000000000000, 0x0A0E000000000000, 0x141C000000000000,
    0x2838000000000000, 0x5070000000000000, 0xA0E0000000000000, 0x40C0000000000000
};

// pre calculated lookup table for pawn attacks
static constexpr Bitboard PAWN_ATTACKS_TABLE[2][64] = {
    
    // white pawn attacks
    { 0x200, 0x500, 0xa00, 0x1400,
      0x2800, 0x5000, 0xa000, 0x4000,
      0x20000, 0x50000, 0xa0000, 0x140000,
      0x280000, 0x500000, 0xa00000, 0x400000,
      0x2000000, 0x5000000, 0xa000000, 0x14000000,
      0x28000000, 0x50000000, 0xa0000000, 0x40000000,
      0x200000000, 0x500000000, 0xa00000000, 0x1400000000,
      0x2800000000, 0x5000000000, 0xa000000000, 0x4000000000,
      0x20000000000, 0x50000000000, 0xa0000000000, 0x140000000000,
      0x280000000000, 0x500000000000, 0xa00000000000, 0x400000000000,
      0x2000000000000, 0x5000000000000, 0xa000000000000, 0x14000000000000,
      0x28000000000000, 0x50000000000000, 0xa0000000000000, 0x40000000000000,
      0x200000000000000, 0x500000000000000, 0xa00000000000000, 0x1400000000000000,
      0x2800000000000000, 0x5000000000000000, 0xa000000000000000, 0x4000000000000000,
      0x0, 0x0, 0x0, 0x0,
      0x0, 0x0, 0x0, 0x0 }, 
    
    // black pawn attacks
    { 0x0, 0x0, 0x0, 0x0,
      0x0, 0x0, 0x0, 0x0,
      0x2, 0x5, 0xa, 0x14,
      0x28, 0x50, 0xa0, 0x40,
      0x200, 0x500, 0xa00, 0x1400,
      0x2800, 0x5000, 0xa000, 0x4000,
      0x20000, 0x50000, 0xa0000, 0x140000,
      0x280000, 0x500000, 0xa00000, 0x400000,
      0x2000000, 0x5000000, 0xa000000, 0x14000000,
      0x28000000, 0x50000000, 0xa0000000, 0x40000000,
      0x200000000, 0x500000000, 0xa00000000, 0x1400000000,
      0x2800000000, 0x5000000000, 0xa000000000, 0x4000000000,
      0x20000000000, 0x50000000000, 0xa0000000000, 0x140000000000,
      0x280000000000, 0x500000000000, 0xa00000000000, 0x400000000000,
      0x2000000000000, 0x5000000000000, 0xa000000000000, 0x14000000000000,
      0x28000000000000, 0x50000000000000, 0xa0000000000000, 0x40000000000000
    }
};


// Hyperbola Quintessence algorithm (for sliding pieces)
inline Bitboard Board::hyp_quint(Square square, Bitboard occ, Bitboard mask) {
    return (((mask & occ) - SQUARE_BB[square] * 2) ^
        reverse(reverse(mask & occ) - reverse(SQUARE_BB[square]) * 2)) & mask;
}
// get Pawn push bitboard
template <Color c>
inline Bitboard Board::GetPawnPush(Square sq) {
    return (c == White) ? (SQUARE_BB[sq + 8]) : (SQUARE_BB[sq - 8]);
}

// get absolute pawn attacks from lookup table
template <Color c>
inline Bitboard Board::GetPawnAttacks(Square square) {
    return PAWN_ATTACKS_TABLE[c][square];
}

// get absolute knight attacks from lookup table
inline Bitboard Board::GetKnightAttacks(Square square) {
    return KNIGHT_ATTACKS_TABLE[square];
}
 
// get bishop attacks using Hyperbola Quintessence
inline Bitboard Board::GetBishopAttacks(Square square, Bitboard occ) {
    return hyp_quint(square, occ, MASK_DIAGONAL[diagonal_of(square)]) |
           hyp_quint(square, occ, MASK_ANTI_DIAGONAL[anti_diagonal_of(square)]);
}
 
// get rook attacks using Hyperbola Quintessence
inline Bitboard Board::GetRookAttacks(Square square, Bitboard occ) {
    return hyp_quint(square, occ, MASK_FILE[file_of(square)]) |
           hyp_quint(square, occ, MASK_RANK[rank_of(square)]);
}

// get queen attacks using Hyperbola Quintessence
inline Bitboard Board::GetQueenAttacks(Square square, Bitboard occ) {
    return GetBishopAttacks(square, occ) | GetRookAttacks(square, occ);
}
  
// get absolute king attacks from lookup table
inline Bitboard Board::GetKingAttacks(Square square) {
    return KING_ATTACKS_TABLE[square];
}

template <Color c> 
inline Bitboard Board::doCheckmask(Square sq){
    Bitboard us = (c == White) ? occupancyWhite : occupancyBlack;
    Bitboard them = (c == White) ? occupancyBlack : occupancyWhite;
    Bitboard checks = 0ULL;
    Bitboard pawn_attack    = GetPawnAttacks<c>(sq);
    Bitboard knight_attack  = GetKnightAttacks(sq);
    Bitboard bishop_attack  = GetBishopAttacks(sq, us | them) & ~us;
    Bitboard rook_attack    = GetRookAttacks(sq, us | them) & ~us;

    Bitboard pawn_mask      = pawn_attack & Pawns<~c>();
    Bitboard knight_mask    = knight_attack & Knights<~c>();
    Bitboard bishop_mask    = bishop_attack & (Bishops<~c>() | Queens<~c>());
    Bitboard rook_mask      = rook_attack & (Rooks<~c>() | Queens<~c>());
    doubleCheck = 0;
    if (pawn_mask) {
        checks   |= pawn_mask;
        doubleCheck++;
    }
    if (knight_mask){
        checks   |= knight_mask;
        doubleCheck++;
    }
    if (bishop_mask){
        Square index = bsf(bishop_mask);
        checks |= SQUARES_BETWEEN_BB[sq][index] | SQUARE_BB[index];
        doubleCheck++;
    } 
    if (rook_mask){
        Square index = bsf(rook_mask);
        checks |= SQUARES_BETWEEN_BB[sq][index] | SQUARE_BB[index];
        doubleCheck++;
    }
    return checks;
}

template <Color c> 
inline void Board::create_pins(Square sq){
    Bitboard us = (c == White) ? occupancyWhite : occupancyBlack;
    Bitboard them = (c == White) ? occupancyBlack : occupancyWhite;
    Bitboard rook_attack   = GetRookAttacks(sq, them);
    Bitboard bishop_attack = GetBishopAttacks(sq, them);
    Bitboard rook_mask     = rook_attack & (Rooks<~c>() | Queens<~c>());
    Bitboard bishop_mask   = bishop_attack & (Bishops<~c>() | Queens<~c>());
    Bitboard rook_pin      = 0ULL;
    Bitboard bishop_pin    = 0ULL;
    pinMaskHV = 0ULL;
    pinMaskD  = 0ULL;
    //SQUARES_BETWEEN_BB[SQ][index] | SQUARES_BB[index]
    while (rook_mask){
        Square index = poplsb(rook_mask);
        Bitboard possible_pin = (SQUARES_BETWEEN_BB[sq][index] | SQUARE_BB[index]) | (1ULL << index);
        if (popCount(possible_pin & us) == 1)
            rook_pin |= possible_pin;
    }
    while (bishop_mask){
        Square index = poplsb(bishop_mask);
        Bitboard possible_pin = (SQUARES_BETWEEN_BB[sq][index] | SQUARE_BB[index]) | (1ULL << index);
        if (popCount(possible_pin & us) == 1)
            bishop_pin |= possible_pin;
    }
    pinMaskHV = rook_pin;
    pinMaskD  = bishop_pin;
}

template <Color c> 
inline void Board::init(Square sq){
    occupancyWhite = allPieces<White>();
    occupancyBlack = allPieces<Black>();
    occupancyAll   = occupancyWhite | occupancyBlack;
    Bitboard mask = doCheckmask<c>(sq);
    checkMask = mask ? mask : 18446744073709551615ULL;
    create_pins<c>(sq);
}

template <Color c> 
inline Bitboard Board::LegalPawnMoves(Square sq){
    Bitboard enemy = c==White ? occupancyBlack : occupancyWhite;
    if (doubleCheck == 2) return 0ULL;
    if (pinMaskD & (1ULL << sq)) return GetPawnAttacks<c>(sq) & pinMaskD & checkMask & enemy;

    Bitboard not_all = ~occupancyWhite & ~occupancyBlack;
    Bitboard attacks = GetPawnAttacks<c>(sq);
    Bitboard push = GetPawnPush<c>(sq) & not_all;
    Bitboard push2 = c==White && rank_of(sq) == 1 ?
                     push |= (push << 8) & not_all : c==Black && rank_of(sq) == 6 ?
                     push |= (push >> 8) & not_all : 0ULL;
    if (pinMaskHV & (1ULL << sq)) return (push | push2) & pinMaskHV & checkMask;
    int8_t offset = c==White ? -8 : 8;
    if (checkMask != 18446744073709551615ULL && attacks & (1ULL << enpassantSquare) && checkMask & (1ULL << (enpassantSquare + offset))) return (GetPawnAttacks<c>(sq) & (1ULL << enpassantSquare));
    if (checkMask != 18446744073709551615ULL) return ((GetPawnAttacks<c>(sq) & enemy) | push | push2) & checkMask;

    Bitboard moves = ((attacks & enemy) | push | push2) & checkMask;
    if (enpassantSquare != NO_SQ && squareDistance(sq, enpassantSquare) == 1) {
        // Horizontal Ep rook pins our pawn through another pawn, our pawn can push but not take enpassant 
        if ((1ULL << enpassantSquare) & attacks){
            removePiece(makePiece<c>(Pawn), sq);
            removePiece(makePiece<~c>(Pawn), Square(enpassantSquare + offset));
            placePiece(makePiece<c>(Pawn), enpassantSquare);
            if (!isSquareAttacked<~c>(KingSq<c>())) moves |= (1ULL << enpassantSquare);
            removePiece(makePiece<c>(Pawn), enpassantSquare);
            placePiece(makePiece<c>(Pawn), sq);
            placePiece(makePiece<~c>(Pawn), Square(enpassantSquare + offset));         
        }

    }
    return moves;
}

template <Color c> 
inline Bitboard Board::LegalKnightMoves(Square sq){
    if (doubleCheck == 2) return 0ULL;
    if ((pinMaskHV | pinMaskD) & (1ULL << sq)) return 0ULL;
    return GetKnightAttacks(sq) & EnemyEmpty<c>() & checkMask;
}

template <Color c> 
inline Bitboard Board::LegalBishopMoves(Square sq){
    if (doubleCheck == 2) return 0ULL;
    if (pinMaskHV & (1ULL << sq)) return 0ULL;
    if (pinMaskD & (1ULL << sq)) return GetBishopAttacks(sq, occupancyWhite | occupancyBlack) & EnemyEmpty<c>() & checkMask & pinMaskD;
    // Bitboard containing all occupancies
    return GetBishopAttacks(sq, occupancyAll) & EnemyEmpty<c>() & checkMask;
}

template <Color c> 
inline Bitboard Board::LegalRookMoves(Square sq){
    if (doubleCheck == 2) return 0ULL;
    if (pinMaskD & (1ULL << sq)) return 0ULL;
    if (pinMaskHV & (1ULL << sq)) return GetRookAttacks(sq, occupancyWhite | occupancyBlack) & EnemyEmpty<c>() & checkMask & pinMaskHV;
    return GetRookAttacks(sq, occupancyAll) & EnemyEmpty<c>() & checkMask;
}

template <Color c> 
inline Bitboard Board::LegalQueenMoves(Square sq){
    if (doubleCheck == 2) return 0ULL;
    return LegalRookMoves<c>(sq) | LegalBishopMoves<c>(sq);
}

template <Color c> 
inline Bitboard Board::LegalKingMoves(Square sq){
    Bitboard king_moves = GetKingAttacks(sq) & EnemyEmpty<c>();

    // remove king
    removePiece(makePiece<c>(King), sq);

    Bitboard legal_king = 0ULL;
    while (king_moves){
        Square index = poplsb(king_moves);
        if (isSquareAttacked<~c>(index)){
            legal_king |= 0ULL;
        }
        else
            legal_king |= (1ULL << index);
        
    }
    
    // restore king
    placePiece(makePiece<c>(King), sq);

    Bitboard castlingMoves = 0ULL;
    bool inCheck = 18446744073709551615ULL != checkMask;
    if (!inCheck){
        if (castlingRights & whiteKingSideCastling && sideToMove == White &&
            !(occupancyAll & (1ULL << SQ_F1)) &&
            !(occupancyAll & (1ULL << SQ_G1)) &&
            (1ULL << SQ_H1 & Rooks<White>()) &&
            !(isSquareAttacked<~c>(SQ_F1)) &&
            !(isSquareAttacked<~c>(SQ_G1)))
        {
            castlingMoves |= (1ULL << SQ_G1);
        }

        if (castlingRights & whiteQueenSideCastling && sideToMove == White &&
            !(occupancyAll & (1ULL << SQ_D1)) &&
            !(occupancyAll & (1ULL << SQ_C1)) &&
            !(occupancyAll & (1ULL << SQ_B1)) &&
            (1ULL << SQ_A1 & Rooks<White>()) &&
            !(isSquareAttacked<~c>(SQ_D1)) &&
            !(isSquareAttacked<~c>(SQ_C1)))
        {
            castlingMoves |= (1ULL << SQ_C1);
        }

        if (castlingRights & blackKingSideCastling && sideToMove == Black &&
            !(occupancyAll & (1ULL << SQ_F8)) &&
            !(occupancyAll & (1ULL << SQ_G8)) &&
            (1ULL << SQ_H8 & Rooks<Black>()) &&
            !(isSquareAttacked<~c>(SQ_F8)) &&
            !(isSquareAttacked<~c>(SQ_G8)))
        {
            castlingMoves |= (1ULL << SQ_G8);
        }
        if (castlingRights & blackQueenSideCastling && sideToMove == Black &&
            !(occupancyAll & (1ULL << SQ_D8)) &&
            !(occupancyAll & (1ULL << SQ_C8)) &&
            !(occupancyAll & (1ULL << SQ_B8)) &&
            (1ULL << SQ_A8 & Rooks<Black>()) &&
            !(isSquareAttacked<~Black>(SQ_D8)) &&
            !(isSquareAttacked<~Black>(SQ_C8)))
        {
            castlingMoves |= (1ULL << SQ_C8);
        }        
    }
    return legal_king | castlingMoves;
}

// function that returns a list of legal moves
template <Color c> 
Moves Board::generateLegalMoves() {
    // init move list
    Moves moveList{};
    init<c>(KingSq<c>());

    Bitboard pawn_mask = Pawns<c>();
    Bitboard knight_mask = Knights<c>();
    Bitboard bishop_mask = Bishops<c>();
    Bitboard rook_mask = Rooks<c>();
    Bitboard queen_mask = Queens<c>();
    Bitboard king_mask = Kings<c>();
    Bitboard enemy = c==White ? occupancyBlack : occupancyWhite;
    if (doubleCheck < 2){
        while (pawn_mask){
            Square source = poplsb(pawn_mask);
            Bitboard moves = LegalPawnMoves<c>(source);
            while (moves){
                Square target = poplsb(moves);
                uint32_t capture = ((1ULL << target) & enemy) ? 1 : 0;
                if (rank_of(target) == 7 || rank_of(target) == 0){
                    moveList.Add(Move(source, target, makePiece<c>(Pawn), makePiece<c>(Queen), capture));
                    moveList.Add(Move(source, target, makePiece<c>(Pawn), makePiece<c>(Rook), capture));
                    moveList.Add(Move(source, target, makePiece<c>(Pawn), makePiece<c>(Bishop), capture));
                    moveList.Add(Move(source, target, makePiece<c>(Pawn), makePiece<c>(Knight), capture));
                }
                else if (std::abs(source - target) == 16)
                    moveList.Add(Move(source, target, makePiece<c>(Pawn), None, 0, 1, 0));
                else if (target == enpassantSquare)
                    moveList.Add(Move(source, target, makePiece<c>(Pawn), None, 1, 0, 1));
                else
                    moveList.Add(Move(source, target, makePiece<c>(Pawn), None, capture, 0, 0));
            }
        }
        while (knight_mask){
            Square source = poplsb(knight_mask);
            Bitboard moves = LegalKnightMoves<c>(source);
            while (moves){
                Square target = poplsb(moves);
                uint32_t capture = ((1ULL << target) & enemy) ? 1 : 0;
                moveList.Add(Move(source, target, makePiece<c>(Knight), None, capture));
            }
        }
        while (bishop_mask){
            Square source = poplsb(bishop_mask);
            Bitboard moves = LegalBishopMoves<c>(source);
            while (moves){
                Square target = poplsb(moves);
                uint32_t capture = ((1ULL << target) & enemy) ? 1 : 0;
                moveList.Add(Move(source, target, makePiece<c>(Bishop), None, capture));
            }
        }
        while (rook_mask){
            Square source = poplsb(rook_mask);
            Bitboard moves = LegalRookMoves<c>(source);
            while (moves){
                Square target = poplsb(moves);
                uint32_t capture = ((1ULL << target) & enemy) ? 1 : 0;
                moveList.Add(Move(source, target, makePiece<c>(Rook), None, capture));
            }
        }
        while (queen_mask){
            Square source = poplsb(queen_mask);
            Bitboard moves = LegalQueenMoves<c>(source);
            while (moves){
                Square target = poplsb(moves);
                uint32_t capture = ((1ULL << target) & enemy) ? 1 : 0;
                moveList.Add(Move(source, target, makePiece<c>(Queen), None, capture));
            }
        }
    }

    Square source = poplsb(king_mask);
    Bitboard moves = LegalKingMoves<c>(source);
    while (moves){
        Square target = poplsb(moves);
        uint32_t capture = ((1ULL << target) & enemy) ? 1 : 0;
        if (target == SQ_G1 && source == SQ_E1)
            moveList.Add(Move(source, target, WhiteKing, None, 0, 0, 0, 1));
        else if (target == SQ_C1 && source == SQ_E1)
            moveList.Add(Move(source, target, WhiteKing, None, 0, 0, 0, 1));
        else if (target == SQ_G8 && source == SQ_E8)
            moveList.Add(Move(source, target, BlackKing, None, 0, 0, 0, 1));
        else if (target == SQ_C8 && source == SQ_E8)
            moveList.Add(Move(source, target, BlackKing, None, 0, 0, 0, 1));
        else
            moveList.Add(Move(source, target, makePiece<c>(King), None, capture));
    }

    return moveList;
}

Piece Board::getPiece(Square sq){
    return board[sq];
}
template <Color c> 
void Board::makemove(Move& move){
    bool capture = move.capture();
    Piece capturedPiece = capture ? board[move.target()] : None;
    // Safe important board information
    State safeState{};
    safeState.enpassantCopy = enpassantSquare;
    safeState.castlingRightsCopy = castlingRights;
    safeState.capturedPiece = capturedPiece;
    storeInfo[storeCount] = safeState;
    storeCount++;

    Piece piece = move.piece();
    Piece promoted = move.promoted();
    Square source = move.source();
    Square target = move.target();
    bool enpassant = move.enpassant();
    
    // update castling rights
    if (piece == makePiece<c>(King)){
        if (source == SQ_E1 && target == SQ_G1 && castlingRights & whiteKingSideCastling){
            castlingRights &= ~whiteKingSideCastling;
            castlingRights &= ~whiteQueenSideCastling;
            removePiece(WhiteRook, SQ_H1);
            placePiece(WhiteRook, SQ_F1);
        }  
        else if (source == SQ_E8 && target == SQ_G8 && castlingRights & blackKingSideCastling){
            castlingRights &= ~blackKingSideCastling;
            castlingRights &= ~blackQueenSideCastling;
            removePiece(BlackRook, SQ_H8);
            placePiece(BlackRook, SQ_F8);
        }
        else if (source == SQ_E1 && target == SQ_C1 && castlingRights & whiteQueenSideCastling){
            castlingRights &= ~whiteQueenSideCastling;
            castlingRights &= ~whiteKingSideCastling;
            removePiece(WhiteRook, SQ_A1);
            placePiece(WhiteRook, SQ_D1);
        } 
        else if (source == SQ_E8 && target == SQ_C8 && castlingRights & blackQueenSideCastling){
            castlingRights &= ~blackQueenSideCastling;
            castlingRights &= ~blackKingSideCastling;
            removePiece(BlackRook, SQ_A8);
            placePiece(BlackRook, SQ_D8);
        }
    }
    if (piece == WhiteKing && sideToMove == White){
        castlingRights &= ~whiteKingSideCastling;
        castlingRights &= ~whiteQueenSideCastling;
    }
    if (piece == BlackKing && sideToMove == Black){
        castlingRights &= ~blackKingSideCastling;
        castlingRights &= ~blackQueenSideCastling;
    }
    // rook move loses castle rights
    if (piece == WhiteRook){
        if (source == SQ_A1)
            castlingRights &= ~whiteQueenSideCastling;
        else if (source == SQ_H1)
            castlingRights &= ~whiteKingSideCastling;
    }
    else if (piece == BlackRook){
        if (source == SQ_A8)
            castlingRights &= ~blackQueenSideCastling;
        else if (source == SQ_H8)
            castlingRights &= ~blackKingSideCastling;
    }
    // Rook capture loses castle rights
    if (capture && (1ULL << target & Rooks<~c>())){
        if (target == SQ_A1)
            castlingRights &= ~whiteQueenSideCastling;
        else if (target == SQ_H1)
            castlingRights &= ~whiteKingSideCastling;
    }
    else if (capture && (1ULL << target & Rooks<~c>())){
        if (target == SQ_A8)
            castlingRights &= ~blackQueenSideCastling;
        else if (target == SQ_H8)
            castlingRights &= ~blackKingSideCastling;
    }

    // enpassant capture
    if (target == enpassantSquare && enpassant == 1){
        int8_t offset = sideToMove == White ? -8 : 8;
        removePiece(makePiece<~c>(Pawn), Square(target + offset));
    }

    // update enpassant square
    enpassantSquare = NO_SQ;
    if (piece == makePiece<c>(Pawn) && std::abs(source - target) == 16){
        int8_t offset = sideToMove == White ? -8 : 8;
        Bitboard epMask = GetPawnAttacks<c>(Square(target + offset));
        if (epMask & Pawns<~c>()){
            enpassantSquare = Square(target + offset);
        }
    }
    


    // update half move clock
    // if (piece == makePiece(Pawn, sideToMove))
    //     halfMoveClock = 0;

    if (capture == 1 && enpassant != 1)
        removePiece(capturedPiece, target);

    removePiece(piece, source);
    placePiece(piece, target);

    if (promoted != None){
        removePiece(piece, target);
        placePiece(promoted, target);
    }

    // Switch sides
    sideToMove = ~sideToMove;
}
template <Color c> 
void Board::unmakemove(Move& move){
    // Retrive important board information
    storeCount--;
    State safeState = storeInfo[storeCount];
    enpassantSquare = safeState.enpassantCopy;
    castlingRights = safeState.castlingRightsCopy;

    sideToMove = ~sideToMove;

    Piece piece = move.piece();
    Piece promoted = move.promoted();
    Square source = move.source();
    Square target = move.target();
    bool enpassant = move.enpassant();

    removePiece(piece, target);
    placePiece(piece, source);

    if (enpassant && piece == WhitePawn) {
        placePiece(BlackPawn, Square((int)enpassantSquare - 8));
    }
    if (enpassant && piece == BlackPawn) {
        placePiece(WhitePawn, Square((int)enpassantSquare + 8));
    }
    if (promoted != None) {
        removePiece(promoted, target);
    }
    if (move.capture() == 1 && !enpassant) {
        placePiece(safeState.capturedPiece, target);
    }
    if (piece == WhiteKing && move.castling() == 1) {
        if (target == SQ_G1 && source == SQ_E1) {
            removePiece(WhiteRook, SQ_F1);
            placePiece(WhiteRook, SQ_H1);
        }
        if (target == SQ_C1 && source == SQ_E1) {
            removePiece(WhiteRook, SQ_D1);
            placePiece(WhiteRook, SQ_A1);
        }
    }
    if (piece == BlackKing && move.castling() == 1){
        if (target == SQ_G8 && source == SQ_E8) {
            removePiece(BlackRook, SQ_F8);
            placePiece(BlackRook, SQ_H8);
        }
        if (target == SQ_C8 && source == SQ_E8) {
            removePiece(BlackRook, SQ_D8);
            placePiece(BlackRook, SQ_A8);
            
        }
    }
}

// checks if the square is attacked by the specified color
template <Color c> 
bool Board::isSquareAttacked(Square sq) {
    if (sq != NO_SQ) {
        if (GetPawnAttacks<~c>(sq) & Pawns<c>())                                  return true;
        if (GetKnightAttacks(sq) & Knights<c>())                                        return true;
        if (GetBishopAttacks(sq, allPieces<White>() | allPieces<Black>()) & Bishops<c>())   return true;
        if (GetRookAttacks  (sq, allPieces<White>() | allPieces<Black>()) & Rooks<c>())     return true;
        if (GetQueenAttacks (sq, allPieces<White>() | allPieces<Black>()) & Queens<c>())    return true;
        if (GetKingAttacks  (sq)                                          & Kings<c>())     return true;
    }
    return false;
}



}