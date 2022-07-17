#include <iostream>
#include <cstdint>
#include <vector>
#include <string>
#include <cstring>
#include <unordered_map>
#include <bitset>
#include <cmath>
#include <chrono>
#include <algorithm>

#include "chess.hpp"

namespace Chess {

/**********************************\
 ==================================
             Bitboards
 ==================================
\**********************************/

//returns reversed bitboard (rotate 180 degrees)
inline Bitboard reverse(Bitboard bb) {
    bb = (bb & 0x5555555555555555) << 1 | ((bb >> 1) & 0x5555555555555555);
    bb = (bb & 0x3333333333333333) << 2 | ((bb >> 2) & 0x3333333333333333);
    bb = (bb & 0x0f0f0f0f0f0f0f0f) << 4 | ((bb >> 4) & 0x0f0f0f0f0f0f0f0f);
    bb = (bb & 0x00ff00ff00ff00ff) << 8 | ((bb >> 8) & 0x00ff00ff00ff00ff);

    return (bb << 48) | ((bb & 0xffff0000) << 16) | ((bb >> 16) & 0xffff0000) | (bb >> 48);
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

Color piece_color(Piece p){
    return Color(p / 6);
}

int squareDistance(Square a, Square b) {
    return std::max(std::abs(file_of(a) - file_of(b)), std::abs(rank_of(a) - rank_of(b)));
}

/**********************************\
 ==================================
               Moves
 ==================================
\**********************************/

void printMove(Move move) {
    std::cout << "Move: " << squareToString[from(move)] << squareToString[to(move)] << " |";
    std::cout << " Piece: " << signed(piece_type(move)) << " |";
    std::cout << " Promoted: " << promoted(move) << " |";
    std::cout << std::endl;
}

/**********************************\
 ==================================
               Board
 ==================================
\**********************************/

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

uint8_t Board::ply() {
    return halfMoveClock;
}

uint16_t Board::fullmoves() {
    return fullMoveCounter * 0.5;
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

Piece Board::piece_at(Square sq){
    return board[sq];
}

PieceType Board::piece_type_at(Square sq){
    return PieceType(board[sq]);
}


// place a piece on a particular square
void Board::placePiece(Piece piece, Square sq) {
    PiecesBB[piece] |= SQUARE_BB[sq];
    board[sq] = piece;
}

// remove a piece from a particular square
void Board::removePiece(Piece piece, Square sq) {
    PiecesBB[piece] &= ~SQUARE_BB[sq];
    board[sq] = None;
}

bool Board::isCheck(Color c)
{
    if (c == White)
        return isCheck<White>();
    return isCheck<Black>();
}

/**********************************\
 ==================================
         Move generation
 ==================================
\**********************************/

// Hyperbola Quintessence algorithm (for sliding pieces)
Bitboard Board::hyp_quint(Square square, Bitboard occ, Bitboard mask) {
    return (((mask & occ) - SQUARE_BB[square] * 2) ^
        reverse(reverse(mask & occ) - reverse(SQUARE_BB[square]) * 2)) & mask;
}

// get absolute knight attacks from lookup table
Bitboard Board::GetKnightAttacks(Square square) {
    return KNIGHT_ATTACKS_TABLE[square];
}
 
// get bishop attacks using Hyperbola Quintessence
Bitboard Board::GetBishopAttacks(Square square, Bitboard occ) {
    return hyp_quint(square, occ, MASK_DIAGONAL[diagonal_of(square)]) |
           hyp_quint(square, occ, MASK_ANTI_DIAGONAL[anti_diagonal_of(square)]);
}
 
// get rook attacks using Hyperbola Quintessence
Bitboard Board::GetRookAttacks(Square square, Bitboard occ) {
    return hyp_quint(square, occ, MASK_FILE[file_of(square)]) |
           hyp_quint(square, occ, MASK_RANK[rank_of(square)]);
}

// get queen attacks using Hyperbola Quintessence
Bitboard Board::GetQueenAttacks(Square square, Bitboard occ) {
    return GetBishopAttacks(square, occ) | GetRookAttacks(square, occ);
}

// get absolute king attacks from lookup table
Bitboard Board::GetKingAttacks(Square square) {
    return KING_ATTACKS_TABLE[square];
}

Moves Board::legal_moves()
{
    if (sideToMove == White)
        return generateLegalMoves<White>();
    else
        return generateLegalMoves<Black>();
}

void Board::make_move(Move& move)
{
    if (sideToMove == White)
        makemove<White>(move);
    else
        makemove<Black>(move);
}
void Board::unmake_move(Move& move) 
{
    if (sideToMove == White)
        unmakemove<Black>(move);
    else
        unmakemove<White>(move);
}
/**********************************\
 ==================================
               Perft
 ==================================
\**********************************/

namespace Testing {

std::vector<PerftPosition> testPosition1 = {
    PerftPosition(POSITION_1, 20, 1),
    PerftPosition(POSITION_1, 400, 2),
    PerftPosition(POSITION_1, 8902, 3),
    PerftPosition(POSITION_1, 197281, 4),
    PerftPosition(POSITION_1, 4865609, 5),
    PerftPosition(POSITION_1, 119060324, 6)
};

std::vector<PerftPosition> testPosition2 = {
    PerftPosition(POSITION_2, 48, 1),
    PerftPosition(POSITION_2, 2039, 2),
    PerftPosition(POSITION_2, 97862, 3),
    PerftPosition(POSITION_2, 4085603, 4),
    PerftPosition(POSITION_2, 193690690, 5)
};

std::vector<PerftPosition> testPosition3 = {
    PerftPosition(POSITION_3, 14, 1),
    PerftPosition(POSITION_3, 191, 2),
    PerftPosition(POSITION_3, 2812, 3),
    PerftPosition(POSITION_3, 43238, 4),
    PerftPosition(POSITION_3, 674624, 5),
    PerftPosition(POSITION_3, 11030083, 6),
    PerftPosition(POSITION_3, 178633661, 7)
};

std::vector<PerftPosition> testPosition4 = {
    PerftPosition(POSITION_4, 6, 1),
    PerftPosition(POSITION_4, 264, 2),
    PerftPosition(POSITION_4, 9467, 3),
    PerftPosition(POSITION_4, 422333, 4),
    PerftPosition(POSITION_4, 15833292, 5)
};

std::vector<PerftPosition> testPosition5 = {
    PerftPosition(POSITION_5, 44, 1),
    PerftPosition(POSITION_5, 1486, 2),
    PerftPosition(POSITION_5, 62379, 3),
    PerftPosition(POSITION_5, 2103487, 4),
    PerftPosition(POSITION_5, 89941194, 5)
};

std::vector<PerftPosition> testPosition6 = {
    PerftPosition(POSITION_6, 46, 1),
    PerftPosition(POSITION_6, 2079, 2),
    PerftPosition(POSITION_6, 89890, 3),
    PerftPosition(POSITION_6, 3894594, 4),
    PerftPosition(POSITION_6, 164075551, 5)
};

bool PerftTesting::RunTestOnPosition(std::vector<PerftPosition> positions, std::string fen) {
    auto t1 = std::chrono::high_resolution_clock::now();
    board.parseFEN(fen);
    for (PerftPosition position : positions) {
        uint64_t nodes = PerftTesting::Driver<White>(position.depth);
        if (nodes != position.nodes) {
            std::cout << "Failed: " << position.fen << " Depth " << position.depth << std::endl;
            std::cout << "Got: " << nodes << " Expected: " << position.nodes << std::endl;
            return false;
        }
        std::cout << "Passed: " << nodes << std::endl;
    }
    auto t2 = std::chrono::high_resolution_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
    std::cout << "Time: " << ms.count() << "ms" << "\n\n";
    return true;
}


void PerftTesting::RunPerftTest() {
    std::cout << "BEGIN PERFT TESTING:" << std::endl;
    bool PASSED = true;

    auto t1 = std::chrono::high_resolution_clock::now();
    
    PASSED = RunTestOnPosition(testPosition1, POSITION_1);
    PASSED = RunTestOnPosition(testPosition2, POSITION_2);
    PASSED = RunTestOnPosition(testPosition3, POSITION_3);
    PASSED = RunTestOnPosition(testPosition4, POSITION_4);
    PASSED = RunTestOnPosition(testPosition5, POSITION_5);
    PASSED = RunTestOnPosition(testPosition6, POSITION_6);

    if (PASSED) {
        std::cout << "Passed all tests" << std::endl;
    }

    auto t2 = std::chrono::high_resolution_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
    std::cout << "Total time: " << ms.count() << "ms" << std::endl;
} 



} // end of namespace Testing


} // end of namespace Chess