#pragma once

#include <algorithm>
#include <array>
#include <atomic>
#include <bitset>
#include <cassert>
#include <chrono>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "sliders.hpp"

using namespace Chess_Lookup::Fancy;

namespace Chess
{

// *******************
// TYPES DEFINITION
// *******************
#define U64 uint64_t
#define MAX_SQ 64
#define DEFAULT_POS std::string("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1")

enum Movetype : uint8_t
{
    ALL,
    CAPTURE,
    QUIET
};

enum Move : uint16_t
{
    NO_MOVE = 0,
    NULL_MOVE = 65
};

enum Color : uint8_t
{
    White,
    Black,
    NO_COLOR
};

enum Piece : uint8_t
{
    WhitePawn,
    WhiteKnight,
    WhiteBishop,
    WhiteRook,
    WhiteQueen,
    WhiteKing,
    BlackPawn,
    BlackKnight,
    BlackBishop,
    BlackRook,
    BlackQueen,
    BlackKing,
    None
};

enum PieceType : uint8_t
{
    PAWN,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING,
    NONETYPE
};

enum Rank : uint8_t
{
    RANK_1,
    RANK_2,
    RANK_3,
    RANK_4,
    RANK_5,
    RANK_6,
    RANK_7,
    RANK_8
};

enum File : uint8_t
{
    FILE_A,
    FILE_B,
    FILE_C,
    FILE_D,
    FILE_E,
    FILE_F,
    FILE_G,
    FILE_H
};

enum CastlingRight : uint8_t
{
    wk = 1,
    wq = 2,
    bk = 4,
    bq = 8
};

enum Direction : int8_t
{
    NORTH = 8,
    WEST = -1,
    SOUTH = -8,
    EAST = 1,
    NORTH_EAST = 9,
    NORTH_WEST = 7,
    SOUTH_WEST = -9,
    SOUTH_EAST = -7
};

// clang-format off
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

/// @brief convert a square number to a string
const std::string squareToString[MAX_SQ] = {
    "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1",
    "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
    "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
    "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
    "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
    "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
    "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
    "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8",
};

// clang-format on

static constexpr int MAX_PLY = 120;
static constexpr int MAX_MOVES = 128;

static constexpr U64 WK_CASTLE_MASK = (1ULL << SQ_F1) | (1ULL << SQ_G1);
static constexpr U64 WQ_CASTLE_MASK = (1ULL << SQ_D1) | (1ULL << SQ_C1) | (1ULL << SQ_B1);

static constexpr U64 BK_CASTLE_MASK = (1ULL << SQ_F8) | (1ULL << SQ_G8);
static constexpr U64 BQ_CASTLE_MASK = (1ULL << SQ_D8) | (1ULL << SQ_C8) | (1ULL << SQ_B8);

// all 64 bits set
static constexpr U64 DEFAULT_CHECKMASK = 18446744073709551615ULL;

// *******************
// Type operations
// *******************

constexpr Color operator~(Color C)
{
    return Color(C ^ Black);
}

#define INCR_OP_ON(T)                                                                                                  \
    constexpr inline T &operator++(T &p)                                                                               \
    {                                                                                                                  \
        return p = static_cast<T>(static_cast<int>(p) + 1);                                                            \
    }                                                                                                                  \
    constexpr inline T operator++(T &p, int)                                                                           \
    {                                                                                                                  \
        auto old = p;                                                                                                  \
        ++p;                                                                                                           \
        return old;                                                                                                    \
    }

INCR_OP_ON(Piece)
INCR_OP_ON(Square)
INCR_OP_ON(PieceType)
INCR_OP_ON(Rank)
INCR_OP_ON(File)

#undef INCR_OP_ON

#define BASE_OP_ON(T)                                                                                                  \
    inline constexpr Square operator+(Square s, T d)                                                                   \
    {                                                                                                                  \
        return Square(int(s) + int(d));                                                                                \
    }                                                                                                                  \
    inline constexpr Square operator-(Square s, T d)                                                                   \
    {                                                                                                                  \
        return Square(int(s) - int(d));                                                                                \
    }                                                                                                                  \
    inline constexpr Square &operator+=(Square &s, T d)                                                                \
    {                                                                                                                  \
        return s = s + d;                                                                                              \
    }                                                                                                                  \
    inline constexpr Square &operator-=(Square &s, T d)                                                                \
    {                                                                                                                  \
        return s = s - d;                                                                                              \
    }

BASE_OP_ON(Direction)

#undef BASE_OP_ON

// clang-format off

// pre calculated lookup table for pawn attacks
static constexpr U64 PAWN_ATTACKS_TABLE[2][MAX_SQ] = {
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

// pre calculated lookup table for knight attacks
static constexpr U64 KNIGHT_ATTACKS_TABLE[MAX_SQ] = {
    0x0000000000020400, 0x0000000000050800, 0x00000000000A1100, 0x0000000000142200, 0x0000000000284400,
    0x0000000000508800, 0x0000000000A01000, 0x0000000000402000, 0x0000000002040004, 0x0000000005080008,
    0x000000000A110011, 0x0000000014220022, 0x0000000028440044, 0x0000000050880088, 0x00000000A0100010,
    0x0000000040200020, 0x0000000204000402, 0x0000000508000805, 0x0000000A1100110A, 0x0000001422002214,
    0x0000002844004428, 0x0000005088008850, 0x000000A0100010A0, 0x0000004020002040, 0x0000020400040200,
    0x0000050800080500, 0x00000A1100110A00, 0x0000142200221400, 0x0000284400442800, 0x0000508800885000,
    0x0000A0100010A000, 0x0000402000204000, 0x0002040004020000, 0x0005080008050000, 0x000A1100110A0000,
    0x0014220022140000, 0x0028440044280000, 0x0050880088500000, 0x00A0100010A00000, 0x0040200020400000,
    0x0204000402000000, 0x0508000805000000, 0x0A1100110A000000, 0x1422002214000000, 0x2844004428000000,
    0x5088008850000000, 0xA0100010A0000000, 0x4020002040000000, 0x0400040200000000, 0x0800080500000000,
    0x1100110A00000000, 0x2200221400000000, 0x4400442800000000, 0x8800885000000000, 0x100010A000000000,
    0x2000204000000000, 0x0004020000000000, 0x0008050000000000, 0x00110A0000000000, 0x0022140000000000,
    0x0044280000000000, 0x0088500000000000, 0x0010A00000000000, 0x0020400000000000};

// pre calculated lookup table for king attacks
static constexpr U64 KING_ATTACKS_TABLE[MAX_SQ] = {
    0x0000000000000302, 0x0000000000000705, 0x0000000000000E0A, 0x0000000000001C14, 0x0000000000003828,
    0x0000000000007050, 0x000000000000E0A0, 0x000000000000C040, 0x0000000000030203, 0x0000000000070507,
    0x00000000000E0A0E, 0x00000000001C141C, 0x0000000000382838, 0x0000000000705070, 0x0000000000E0A0E0,
    0x0000000000C040C0, 0x0000000003020300, 0x0000000007050700, 0x000000000E0A0E00, 0x000000001C141C00,
    0x0000000038283800, 0x0000000070507000, 0x00000000E0A0E000, 0x00000000C040C000, 0x0000000302030000,
    0x0000000705070000, 0x0000000E0A0E0000, 0x0000001C141C0000, 0x0000003828380000, 0x0000007050700000,
    0x000000E0A0E00000, 0x000000C040C00000, 0x0000030203000000, 0x0000070507000000, 0x00000E0A0E000000,
    0x00001C141C000000, 0x0000382838000000, 0x0000705070000000, 0x0000E0A0E0000000, 0x0000C040C0000000,
    0x0003020300000000, 0x0007050700000000, 0x000E0A0E00000000, 0x001C141C00000000, 0x0038283800000000,
    0x0070507000000000, 0x00E0A0E000000000, 0x00C040C000000000, 0x0302030000000000, 0x0705070000000000,
    0x0E0A0E0000000000, 0x1C141C0000000000, 0x3828380000000000, 0x7050700000000000, 0xE0A0E00000000000,
    0xC040C00000000000, 0x0203000000000000, 0x0507000000000000, 0x0A0E000000000000, 0x141C000000000000,
    0x2838000000000000, 0x5070000000000000, 0xA0E0000000000000, 0x40C0000000000000};

// used for hash generation
static constexpr U64 RANDOM_ARRAY[781] = {
    0x9D39247E33776D41, 0x2AF7398005AAA5C7, 0x44DB015024623547, 0x9C15F73E62A76AE2, 0x75834465489C0C89,
    0x3290AC3A203001BF, 0x0FBBAD1F61042279, 0xE83A908FF2FB60CA, 0x0D7E765D58755C10, 0x1A083822CEAFE02D,
    0x9605D5F0E25EC3B0, 0xD021FF5CD13A2ED5, 0x40BDF15D4A672E32, 0x011355146FD56395, 0x5DB4832046F3D9E5,
    0x239F8B2D7FF719CC, 0x05D1A1AE85B49AA1, 0x679F848F6E8FC971, 0x7449BBFF801FED0B, 0x7D11CDB1C3B7ADF0,
    0x82C7709E781EB7CC, 0xF3218F1C9510786C, 0x331478F3AF51BBE6, 0x4BB38DE5E7219443, 0xAA649C6EBCFD50FC,
    0x8DBD98A352AFD40B, 0x87D2074B81D79217, 0x19F3C751D3E92AE1, 0xB4AB30F062B19ABF, 0x7B0500AC42047AC4,
    0xC9452CA81A09D85D, 0x24AA6C514DA27500, 0x4C9F34427501B447, 0x14A68FD73C910841, 0xA71B9B83461CBD93,
    0x03488B95B0F1850F, 0x637B2B34FF93C040, 0x09D1BC9A3DD90A94, 0x3575668334A1DD3B, 0x735E2B97A4C45A23,
    0x18727070F1BD400B, 0x1FCBACD259BF02E7, 0xD310A7C2CE9B6555, 0xBF983FE0FE5D8244, 0x9F74D14F7454A824,
    0x51EBDC4AB9BA3035, 0x5C82C505DB9AB0FA, 0xFCF7FE8A3430B241, 0x3253A729B9BA3DDE, 0x8C74C368081B3075,
    0xB9BC6C87167C33E7, 0x7EF48F2B83024E20, 0x11D505D4C351BD7F, 0x6568FCA92C76A243, 0x4DE0B0F40F32A7B8,
    0x96D693460CC37E5D, 0x42E240CB63689F2F, 0x6D2BDCDAE2919661, 0x42880B0236E4D951, 0x5F0F4A5898171BB6,
    0x39F890F579F92F88, 0x93C5B5F47356388B, 0x63DC359D8D231B78, 0xEC16CA8AEA98AD76, 0x5355F900C2A82DC7,
    0x07FB9F855A997142, 0x5093417AA8A7ED5E, 0x7BCBC38DA25A7F3C, 0x19FC8A768CF4B6D4, 0x637A7780DECFC0D9,
    0x8249A47AEE0E41F7, 0x79AD695501E7D1E8, 0x14ACBAF4777D5776, 0xF145B6BECCDEA195, 0xDABF2AC8201752FC,
    0x24C3C94DF9C8D3F6, 0xBB6E2924F03912EA, 0x0CE26C0B95C980D9, 0xA49CD132BFBF7CC4, 0xE99D662AF4243939,
    0x27E6AD7891165C3F, 0x8535F040B9744FF1, 0x54B3F4FA5F40D873, 0x72B12C32127FED2B, 0xEE954D3C7B411F47,
    0x9A85AC909A24EAA1, 0x70AC4CD9F04F21F5, 0xF9B89D3E99A075C2, 0x87B3E2B2B5C907B1, 0xA366E5B8C54F48B8,
    0xAE4A9346CC3F7CF2, 0x1920C04D47267BBD, 0x87BF02C6B49E2AE9, 0x092237AC237F3859, 0xFF07F64EF8ED14D0,
    0x8DE8DCA9F03CC54E, 0x9C1633264DB49C89, 0xB3F22C3D0B0B38ED, 0x390E5FB44D01144B, 0x5BFEA5B4712768E9,
    0x1E1032911FA78984, 0x9A74ACB964E78CB3, 0x4F80F7A035DAFB04, 0x6304D09A0B3738C4, 0x2171E64683023A08,
    0x5B9B63EB9CEFF80C, 0x506AACF489889342, 0x1881AFC9A3A701D6, 0x6503080440750644, 0xDFD395339CDBF4A7,
    0xEF927DBCF00C20F2, 0x7B32F7D1E03680EC, 0xB9FD7620E7316243, 0x05A7E8A57DB91B77, 0xB5889C6E15630A75,
    0x4A750A09CE9573F7, 0xCF464CEC899A2F8A, 0xF538639CE705B824, 0x3C79A0FF5580EF7F, 0xEDE6C87F8477609D,
    0x799E81F05BC93F31, 0x86536B8CF3428A8C, 0x97D7374C60087B73, 0xA246637CFF328532, 0x043FCAE60CC0EBA0,
    0x920E449535DD359E, 0x70EB093B15B290CC, 0x73A1921916591CBD, 0x56436C9FE1A1AA8D, 0xEFAC4B70633B8F81,
    0xBB215798D45DF7AF, 0x45F20042F24F1768, 0x930F80F4E8EB7462, 0xFF6712FFCFD75EA1, 0xAE623FD67468AA70,
    0xDD2C5BC84BC8D8FC, 0x7EED120D54CF2DD9, 0x22FE545401165F1C, 0xC91800E98FB99929, 0x808BD68E6AC10365,
    0xDEC468145B7605F6, 0x1BEDE3A3AEF53302, 0x43539603D6C55602, 0xAA969B5C691CCB7A, 0xA87832D392EFEE56,
    0x65942C7B3C7E11AE, 0xDED2D633CAD004F6, 0x21F08570F420E565, 0xB415938D7DA94E3C, 0x91B859E59ECB6350,
    0x10CFF333E0ED804A, 0x28AED140BE0BB7DD, 0xC5CC1D89724FA456, 0x5648F680F11A2741, 0x2D255069F0B7DAB3,
    0x9BC5A38EF729ABD4, 0xEF2F054308F6A2BC, 0xAF2042F5CC5C2858, 0x480412BAB7F5BE2A, 0xAEF3AF4A563DFE43,
    0x19AFE59AE451497F, 0x52593803DFF1E840, 0xF4F076E65F2CE6F0, 0x11379625747D5AF3, 0xBCE5D2248682C115,
    0x9DA4243DE836994F, 0x066F70B33FE09017, 0x4DC4DE189B671A1C, 0x51039AB7712457C3, 0xC07A3F80C31FB4B4,
    0xB46EE9C5E64A6E7C, 0xB3819A42ABE61C87, 0x21A007933A522A20, 0x2DF16F761598AA4F, 0x763C4A1371B368FD,
    0xF793C46702E086A0, 0xD7288E012AEB8D31, 0xDE336A2A4BC1C44B, 0x0BF692B38D079F23, 0x2C604A7A177326B3,
    0x4850E73E03EB6064, 0xCFC447F1E53C8E1B, 0xB05CA3F564268D99, 0x9AE182C8BC9474E8, 0xA4FC4BD4FC5558CA,
    0xE755178D58FC4E76, 0x69B97DB1A4C03DFE, 0xF9B5B7C4ACC67C96, 0xFC6A82D64B8655FB, 0x9C684CB6C4D24417,
    0x8EC97D2917456ED0, 0x6703DF9D2924E97E, 0xC547F57E42A7444E, 0x78E37644E7CAD29E, 0xFE9A44E9362F05FA,
    0x08BD35CC38336615, 0x9315E5EB3A129ACE, 0x94061B871E04DF75, 0xDF1D9F9D784BA010, 0x3BBA57B68871B59D,
    0xD2B7ADEEDED1F73F, 0xF7A255D83BC373F8, 0xD7F4F2448C0CEB81, 0xD95BE88CD210FFA7, 0x336F52F8FF4728E7,
    0xA74049DAC312AC71, 0xA2F61BB6E437FDB5, 0x4F2A5CB07F6A35B3, 0x87D380BDA5BF7859, 0x16B9F7E06C453A21,
    0x7BA2484C8A0FD54E, 0xF3A678CAD9A2E38C, 0x39B0BF7DDE437BA2, 0xFCAF55C1BF8A4424, 0x18FCF680573FA594,
    0x4C0563B89F495AC3, 0x40E087931A00930D, 0x8CFFA9412EB642C1, 0x68CA39053261169F, 0x7A1EE967D27579E2,
    0x9D1D60E5076F5B6F, 0x3810E399B6F65BA2, 0x32095B6D4AB5F9B1, 0x35CAB62109DD038A, 0xA90B24499FCFAFB1,
    0x77A225A07CC2C6BD, 0x513E5E634C70E331, 0x4361C0CA3F692F12, 0xD941ACA44B20A45B, 0x528F7C8602C5807B,
    0x52AB92BEB9613989, 0x9D1DFA2EFC557F73, 0x722FF175F572C348, 0x1D1260A51107FE97, 0x7A249A57EC0C9BA2,
    0x04208FE9E8F7F2D6, 0x5A110C6058B920A0, 0x0CD9A497658A5698, 0x56FD23C8F9715A4C, 0x284C847B9D887AAE,
    0x04FEABFBBDB619CB, 0x742E1E651C60BA83, 0x9A9632E65904AD3C, 0x881B82A13B51B9E2, 0x506E6744CD974924,
    0xB0183DB56FFC6A79, 0x0ED9B915C66ED37E, 0x5E11E86D5873D484, 0xF678647E3519AC6E, 0x1B85D488D0F20CC5,
    0xDAB9FE6525D89021, 0x0D151D86ADB73615, 0xA865A54EDCC0F019, 0x93C42566AEF98FFB, 0x99E7AFEABE000731,
    0x48CBFF086DDF285A, 0x7F9B6AF1EBF78BAF, 0x58627E1A149BBA21, 0x2CD16E2ABD791E33, 0xD363EFF5F0977996,
    0x0CE2A38C344A6EED, 0x1A804AADB9CFA741, 0x907F30421D78C5DE, 0x501F65EDB3034D07, 0x37624AE5A48FA6E9,
    0x957BAF61700CFF4E, 0x3A6C27934E31188A, 0xD49503536ABCA345, 0x088E049589C432E0, 0xF943AEE7FEBF21B8,
    0x6C3B8E3E336139D3, 0x364F6FFA464EE52E, 0xD60F6DCEDC314222, 0x56963B0DCA418FC0, 0x16F50EDF91E513AF,
    0xEF1955914B609F93, 0x565601C0364E3228, 0xECB53939887E8175, 0xBAC7A9A18531294B, 0xB344C470397BBA52,
    0x65D34954DAF3CEBD, 0xB4B81B3FA97511E2, 0xB422061193D6F6A7, 0x071582401C38434D, 0x7A13F18BBEDC4FF5,
    0xBC4097B116C524D2, 0x59B97885E2F2EA28, 0x99170A5DC3115544, 0x6F423357E7C6A9F9, 0x325928EE6E6F8794,
    0xD0E4366228B03343, 0x565C31F7DE89EA27, 0x30F5611484119414, 0xD873DB391292ED4F, 0x7BD94E1D8E17DEBC,
    0xC7D9F16864A76E94, 0x947AE053EE56E63C, 0xC8C93882F9475F5F, 0x3A9BF55BA91F81CA, 0xD9A11FBB3D9808E4,
    0x0FD22063EDC29FCA, 0xB3F256D8ACA0B0B9, 0xB03031A8B4516E84, 0x35DD37D5871448AF, 0xE9F6082B05542E4E,
    0xEBFAFA33D7254B59, 0x9255ABB50D532280, 0xB9AB4CE57F2D34F3, 0x693501D628297551, 0xC62C58F97DD949BF,
    0xCD454F8F19C5126A, 0xBBE83F4ECC2BDECB, 0xDC842B7E2819E230, 0xBA89142E007503B8, 0xA3BC941D0A5061CB,
    0xE9F6760E32CD8021, 0x09C7E552BC76492F, 0x852F54934DA55CC9, 0x8107FCCF064FCF56, 0x098954D51FFF6580,
    0x23B70EDB1955C4BF, 0xC330DE426430F69D, 0x4715ED43E8A45C0A, 0xA8D7E4DAB780A08D, 0x0572B974F03CE0BB,
    0xB57D2E985E1419C7, 0xE8D9ECBE2CF3D73F, 0x2FE4B17170E59750, 0x11317BA87905E790, 0x7FBF21EC8A1F45EC,
    0x1725CABFCB045B00, 0x964E915CD5E2B207, 0x3E2B8BCBF016D66D, 0xBE7444E39328A0AC, 0xF85B2B4FBCDE44B7,
    0x49353FEA39BA63B1, 0x1DD01AAFCD53486A, 0x1FCA8A92FD719F85, 0xFC7C95D827357AFA, 0x18A6A990C8B35EBD,
    0xCCCB7005C6B9C28D, 0x3BDBB92C43B17F26, 0xAA70B5B4F89695A2, 0xE94C39A54A98307F, 0xB7A0B174CFF6F36E,
    0xD4DBA84729AF48AD, 0x2E18BC1AD9704A68, 0x2DE0966DAF2F8B1C, 0xB9C11D5B1E43A07E, 0x64972D68DEE33360,
    0x94628D38D0C20584, 0xDBC0D2B6AB90A559, 0xD2733C4335C6A72F, 0x7E75D99D94A70F4D, 0x6CED1983376FA72B,
    0x97FCAACBF030BC24, 0x7B77497B32503B12, 0x8547EDDFB81CCB94, 0x79999CDFF70902CB, 0xCFFE1939438E9B24,
    0x829626E3892D95D7, 0x92FAE24291F2B3F1, 0x63E22C147B9C3403, 0xC678B6D860284A1C, 0x5873888850659AE7,
    0x0981DCD296A8736D, 0x9F65789A6509A440, 0x9FF38FED72E9052F, 0xE479EE5B9930578C, 0xE7F28ECD2D49EECD,
    0x56C074A581EA17FE, 0x5544F7D774B14AEF, 0x7B3F0195FC6F290F, 0x12153635B2C0CF57, 0x7F5126DBBA5E0CA7,
    0x7A76956C3EAFB413, 0x3D5774A11D31AB39, 0x8A1B083821F40CB4, 0x7B4A38E32537DF62, 0x950113646D1D6E03,
    0x4DA8979A0041E8A9, 0x3BC36E078F7515D7, 0x5D0A12F27AD310D1, 0x7F9D1A2E1EBE1327, 0xDA3A361B1C5157B1,
    0xDCDD7D20903D0C25, 0x36833336D068F707, 0xCE68341F79893389, 0xAB9090168DD05F34, 0x43954B3252DC25E5,
    0xB438C2B67F98E5E9, 0x10DCD78E3851A492, 0xDBC27AB5447822BF, 0x9B3CDB65F82CA382, 0xB67B7896167B4C84,
    0xBFCED1B0048EAC50, 0xA9119B60369FFEBD, 0x1FFF7AC80904BF45, 0xAC12FB171817EEE7, 0xAF08DA9177DDA93D,
    0x1B0CAB936E65C744, 0xB559EB1D04E5E932, 0xC37B45B3F8D6F2BA, 0xC3A9DC228CAAC9E9, 0xF3B8B6675A6507FF,
    0x9FC477DE4ED681DA, 0x67378D8ECCEF96CB, 0x6DD856D94D259236, 0xA319CE15B0B4DB31, 0x073973751F12DD5E,
    0x8A8E849EB32781A5, 0xE1925C71285279F5, 0x74C04BF1790C0EFE, 0x4DDA48153C94938A, 0x9D266D6A1CC0542C,
    0x7440FB816508C4FE, 0x13328503DF48229F, 0xD6BF7BAEE43CAC40, 0x4838D65F6EF6748F, 0x1E152328F3318DEA,
    0x8F8419A348F296BF, 0x72C8834A5957B511, 0xD7A023A73260B45C, 0x94EBC8ABCFB56DAE, 0x9FC10D0F989993E0,
    0xDE68A2355B93CAE6, 0xA44CFE79AE538BBE, 0x9D1D84FCCE371425, 0x51D2B1AB2DDFB636, 0x2FD7E4B9E72CD38C,
    0x65CA5B96B7552210, 0xDD69A0D8AB3B546D, 0x604D51B25FBF70E2, 0x73AA8A564FB7AC9E, 0x1A8C1E992B941148,
    0xAAC40A2703D9BEA0, 0x764DBEAE7FA4F3A6, 0x1E99B96E70A9BE8B, 0x2C5E9DEB57EF4743, 0x3A938FEE32D29981,
    0x26E6DB8FFDF5ADFE, 0x469356C504EC9F9D, 0xC8763C5B08D1908C, 0x3F6C6AF859D80055, 0x7F7CC39420A3A545,
    0x9BFB227EBDF4C5CE, 0x89039D79D6FC5C5C, 0x8FE88B57305E2AB6, 0xA09E8C8C35AB96DE, 0xFA7E393983325753,
    0xD6B6D0ECC617C699, 0xDFEA21EA9E7557E3, 0xB67C1FA481680AF8, 0xCA1E3785A9E724E5, 0x1CFC8BED0D681639,
    0xD18D8549D140CAEA, 0x4ED0FE7E9DC91335, 0xE4DBF0634473F5D2, 0x1761F93A44D5AEFE, 0x53898E4C3910DA55,
    0x734DE8181F6EC39A, 0x2680B122BAA28D97, 0x298AF231C85BAFAB, 0x7983EED3740847D5, 0x66C1A2A1A60CD889,
    0x9E17E49642A3E4C1, 0xEDB454E7BADC0805, 0x50B704CAB602C329, 0x4CC317FB9CDDD023, 0x66B4835D9EAFEA22,
    0x219B97E26FFC81BD, 0x261E4E4C0A333A9D, 0x1FE2CCA76517DB90, 0xD7504DFA8816EDBB, 0xB9571FA04DC089C8,
    0x1DDC0325259B27DE, 0xCF3F4688801EB9AA, 0xF4F5D05C10CAB243, 0x38B6525C21A42B0E, 0x36F60E2BA4FA6800,
    0xEB3593803173E0CE, 0x9C4CD6257C5A3603, 0xAF0C317D32ADAA8A, 0x258E5A80C7204C4B, 0x8B889D624D44885D,
    0xF4D14597E660F855, 0xD4347F66EC8941C3, 0xE699ED85B0DFB40D, 0x2472F6207C2D0484, 0xC2A1E7B5B459AEB5,
    0xAB4F6451CC1D45EC, 0x63767572AE3D6174, 0xA59E0BD101731A28, 0x116D0016CB948F09, 0x2CF9C8CA052F6E9F,
    0x0B090A7560A968E3, 0xABEEDDB2DDE06FF1, 0x58EFC10B06A2068D, 0xC6E57A78FBD986E0, 0x2EAB8CA63CE802D7,
    0x14A195640116F336, 0x7C0828DD624EC390, 0xD74BBE77E6116AC7, 0x804456AF10F5FB53, 0xEBE9EA2ADF4321C7,
    0x03219A39EE587A30, 0x49787FEF17AF9924, 0xA1E9300CD8520548, 0x5B45E522E4B1B4EF, 0xB49C3B3995091A36,
    0xD4490AD526F14431, 0x12A8F216AF9418C2, 0x001F837CC7350524, 0x1877B51E57A764D5, 0xA2853B80F17F58EE,
    0x993E1DE72D36D310, 0xB3598080CE64A656, 0x252F59CF0D9F04BB, 0xD23C8E176D113600, 0x1BDA0492E7E4586E,
    0x21E0BD5026C619BF, 0x3B097ADAF088F94E, 0x8D14DEDB30BE846E, 0xF95CFFA23AF5F6F4, 0x3871700761B3F743,
    0xCA672B91E9E4FA16, 0x64C8E531BFF53B55, 0x241260ED4AD1E87D, 0x106C09B972D2E822, 0x7FBA195410E5CA30,
    0x7884D9BC6CB569D8, 0x0647DFEDCD894A29, 0x63573FF03E224774, 0x4FC8E9560F91B123, 0x1DB956E450275779,
    0xB8D91274B9E9D4FB, 0xA2EBEE47E2FBFCE1, 0xD9F1F30CCD97FB09, 0xEFED53D75FD64E6B, 0x2E6D02C36017F67F,
    0xA9AA4D20DB084E9B, 0xB64BE8D8B25396C1, 0x70CB6AF7C2D5BCF0, 0x98F076A4F7A2322E, 0xBF84470805E69B5F,
    0x94C3251F06F90CF3, 0x3E003E616A6591E9, 0xB925A6CD0421AFF3, 0x61BDD1307C66E300, 0xBF8D5108E27E0D48,
    0x240AB57A8B888B20, 0xFC87614BAF287E07, 0xEF02CDD06FFDB432, 0xA1082C0466DF6C0A, 0x8215E577001332C8,
    0xD39BB9C3A48DB6CF, 0x2738259634305C14, 0x61CF4F94C97DF93D, 0x1B6BACA2AE4E125B, 0x758F450C88572E0B,
    0x959F587D507A8359, 0xB063E962E045F54D, 0x60E8ED72C0DFF5D1, 0x7B64978555326F9F, 0xFD080D236DA814BA,
    0x8C90FD9B083F4558, 0x106F72FE81E2C590, 0x7976033A39F7D952, 0xA4EC0132764CA04B, 0x733EA705FAE4FA77,
    0xB4D8F77BC3E56167, 0x9E21F4F903B33FD9, 0x9D765E419FB69F6D, 0xD30C088BA61EA5EF, 0x5D94337FBFAF7F5B,
    0x1A4E4822EB4D7A59, 0x6FFE73E81B637FB3, 0xDDF957BC36D8B9CA, 0x64D0E29EEA8838B3, 0x08DD9BDFD96B9F63,
    0x087E79E5A57D1D13, 0xE328E230E3E2B3FB, 0x1C2559E30F0946BE, 0x720BF5F26F4D2EAA, 0xB0774D261CC609DB,
    0x443F64EC5A371195, 0x4112CF68649A260E, 0xD813F2FAB7F5C5CA, 0x660D3257380841EE, 0x59AC2C7873F910A3,
    0xE846963877671A17, 0x93B633ABFA3469F8, 0xC0C0F5A60EF4CDCF, 0xCAF21ECD4377B28C, 0x57277707199B8175,
    0x506C11B9D90E8B1D, 0xD83CC2687A19255F, 0x4A29C6465A314CD1, 0xED2DF21216235097, 0xB5635C95FF7296E2,
    0x22AF003AB672E811, 0x52E762596BF68235, 0x9AEBA33AC6ECC6B0, 0x944F6DE09134DFB6, 0x6C47BEC883A7DE39,
    0x6AD047C430A12104, 0xA5B1CFDBA0AB4067, 0x7C45D833AFF07862, 0x5092EF950A16DA0B, 0x9338E69C052B8E7B,
    0x455A4B4CFE30E3F5, 0x6B02E63195AD0CF8, 0x6B17B224BAD6BF27, 0xD1E0CCD25BB9C169, 0xDE0C89A556B9AE70,
    0x50065E535A213CF6, 0x9C1169FA2777B874, 0x78EDEFD694AF1EED, 0x6DC93D9526A50E68, 0xEE97F453F06791ED,
    0x32AB0EDB696703D3, 0x3A6853C7E70757A7, 0x31865CED6120F37D, 0x67FEF95D92607890, 0x1F2B1D1F15F6DC9C,
    0xB69E38A8965C6B65, 0xAA9119FF184CCCF4, 0xF43C732873F24C13, 0xFB4A3D794A9A80D2, 0x3550C2321FD6109C,
    0x371F77E76BB8417E, 0x6BFA9AAE5EC05779, 0xCD04F3FF001A4778, 0xE3273522064480CA, 0x9F91508BFFCFC14A,
    0x049A7F41061A9E60, 0xFCB6BE43A9F2FE9B, 0x08DE8A1C7797DA9B, 0x8F9887E6078735A1, 0xB5B4071DBFC73A66,
    0x230E343DFBA08D33, 0x43ED7F5A0FAE657D, 0x3A88A0FBBCB05C63, 0x21874B8B4D2DBC4F, 0x1BDEA12E35F6A8C9,
    0x53C065C6C8E63528, 0xE34A1D250E7A8D6B, 0xD6B04D3B7651DD7E, 0x5E90277E7CB39E2D, 0x2C046F22062DC67D,
    0xB10BB459132D0A26, 0x3FA9DDFB67E2F199, 0x0E09B88E1914F7AF, 0x10E8B35AF3EEAB37, 0x9EEDECA8E272B933,
    0xD4C718BC4AE8AE5F, 0x81536D601170FC20, 0x91B534F885818A06, 0xEC8177F83F900978, 0x190E714FADA5156E,
    0xB592BF39B0364963, 0x89C350C893AE7DC1, 0xAC042E70F8B383F2, 0xB49B52E587A1EE60, 0xFB152FE3FF26DA89,
    0x3E666E6F69AE2C15, 0x3B544EBE544C19F9, 0xE805A1E290CF2456, 0x24B33C9D7ED25117, 0xE74733427B72F0C1,
    0x0A804D18B7097475, 0x57E3306D881EDB4F, 0x4AE7D6A36EB5DBCB, 0x2D8D5432157064C8, 0xD1E649DE1E7F268B,
    0x8A328A1CEDFE552C, 0x07A3AEC79624C7DA, 0x84547DDC3E203C94, 0x990A98FD5071D263, 0x1A4FF12616EEFC89,
    0xF6F7FD1431714200, 0x30C05B1BA332F41C, 0x8D2636B81555A786, 0x46C9FEB55D120902, 0xCCEC0A73B49C9921,
    0x4E9D2827355FC492, 0x19EBB029435DCB0F, 0x4659D2B743848A2C, 0x963EF2C96B33BE31, 0x74F85198B05A2E7D,
    0x5A0F544DD2B1FB18, 0x03727073C2E134B1, 0xC7F6AA2DE59AEA61, 0x352787BAA0D7C22F, 0x9853EAB63B5E0B35,
    0xABBDCDD7ED5C0860, 0xCF05DAF5AC8D77B0, 0x49CAD48CEBF4A71E, 0x7A4C10EC2158C4A6, 0xD9E92AA246BF719E,
    0x13AE978D09FE5557, 0x730499AF921549FF, 0x4E4B705B92903BA4, 0xFF577222C14F0A3A, 0x55B6344CF97AAFAE,
    0xB862225B055B6960, 0xCAC09AFBDDD2CDB4, 0xDAF8E9829FE96B5F, 0xB5FDFC5D3132C498, 0x310CB380DB6F7503,
    0xE87FBB46217A360E, 0x2102AE466EBB1148, 0xF8549E1A3AA5E00D, 0x07A69AFDCC42261A, 0xC4C118BFE78FEAAE,
    0xF9F4892ED96BD438, 0x1AF3DBE25D8F45DA, 0xF5B4B0B0D2DEEEB4, 0x962ACEEFA82E1C84, 0x046E3ECAAF453CE9,
    0xF05D129681949A4C, 0x964781CE734B3C84, 0x9C2ED44081CE5FBD, 0x522E23F3925E319E, 0x177E00F9FC32F791,
    0x2BC60A63A6F3B3F2, 0x222BBFAE61725606, 0x486289DDCC3D6780, 0x7DC7785B8EFDFC80, 0x8AF38731C02BA980,
    0x1FAB64EA29A2DDF7, 0xE4D9429322CD065A, 0x9DA058C67844F20C, 0x24C0E332B70019B0, 0x233003B5A6CFE6AD,
    0xD586BD01C5C217F6, 0x5E5637885F29BC2B, 0x7EBA726D8C94094B, 0x0A56A5F0BFE39272, 0xD79476A84EE20D06,
    0x9E4C1269BAA4BF37, 0x17EFEE45B0DEE640, 0x1D95B0A5FCF90BC6, 0x93CBE0B699C2585D, 0x65FA4F227A2B6D79,
    0xD5F9E858292504D5, 0xC2B5A03F71471A6F, 0x59300222B4561E00, 0xCE2F8642CA0712DC, 0x7CA9723FBB2E8988,
    0x2785338347F2BA08, 0xC61BB3A141E50E8C, 0x150F361DAB9DEC26, 0x9F6A419D382595F4, 0x64A53DC924FE7AC9,
    0x142DE49FFF7A7C3D, 0x0C335248857FA9E7, 0x0A9C32D5EAE45305, 0xE6C42178C4BBB92E, 0x71F1CE2490D20B07,
    0xF1BCC3D275AFE51A, 0xE728E8C83C334074, 0x96FBF83A12884624, 0x81A1549FD6573DA5, 0x5FA7867CAF35E149,
    0x56986E2EF3ED091B, 0x917F1DD5F8886C61, 0xD20D8C88C8FFE65F, 0x31D71DCE64B2C310, 0xF165B587DF898190,
    0xA57E6339DD2CF3A0, 0x1EF6E6DBB1961EC9, 0x70CC73D90BC26E24, 0xE21A6B35DF0C3AD7, 0x003A93D8B2806962,
    0x1C99DED33CB890A1, 0xCF3145DE0ADD4289, 0xD0E4427A5514FB72, 0x77C621CC9FB3A483, 0x67A34DAC4356550B,
    0xF8D626AAAF278509};

static constexpr U64 castlingKey[16] = {0,
                                        RANDOM_ARRAY[768],
                                        RANDOM_ARRAY[768 + 1],
                                        RANDOM_ARRAY[768] ^ RANDOM_ARRAY[768 + 1],
                                        RANDOM_ARRAY[768 + 2],
                                        RANDOM_ARRAY[768] ^ RANDOM_ARRAY[768 + 2],
                                        RANDOM_ARRAY[768 + 1] ^ RANDOM_ARRAY[768 + 2],
                                        RANDOM_ARRAY[768] ^ RANDOM_ARRAY[768 + 1] ^ RANDOM_ARRAY[768 + 2],
                                        RANDOM_ARRAY[768 + 3],
                                        RANDOM_ARRAY[768] ^ RANDOM_ARRAY[768 + 3],
                                        RANDOM_ARRAY[768 + 1] ^ RANDOM_ARRAY[768 + 3],
                                        RANDOM_ARRAY[768] ^ RANDOM_ARRAY[768 + 1] ^ RANDOM_ARRAY[768 + 3],
                                        RANDOM_ARRAY[768 + 3] ^ RANDOM_ARRAY[768 + 2],
                                        RANDOM_ARRAY[768 + 3] ^ RANDOM_ARRAY[768 + 2] ^ RANDOM_ARRAY[768],
                                        RANDOM_ARRAY[768 + 1] ^ RANDOM_ARRAY[768 + 2] ^ RANDOM_ARRAY[768 + 3],
                                        RANDOM_ARRAY[768 + 1] ^ RANDOM_ARRAY[768 + 2] ^ RANDOM_ARRAY[768 + 3] ^
                                            RANDOM_ARRAY[768]};

// clang-format on

static constexpr int hash_piece[12] = {1, 3, 5, 7, 9, 11, 0, 2, 4, 6, 8, 10};

/// @brief convert a piece to a piecetype
static constexpr PieceType PieceToPieceType[13] = {PAWN,   KNIGHT, BISHOP, ROOK,  QUEEN, KING,    PAWN,
                                                   KNIGHT, BISHOP, ROOK,   QUEEN, KING,  NONETYPE};

// file masks

/// @brief Bitboard of all squares
static constexpr U64 MASK_FILE[8] = {
    0x101010101010101,  0x202020202020202,  0x404040404040404,  0x808080808080808,
    0x1010101010101010, 0x2020202020202020, 0x4040404040404040, 0x8080808080808080,
};

// rank masks

/// @brief Bitboard of all ranks
static constexpr U64 MASK_RANK[8] = {0xff,         0xff00,         0xff0000,         0xff000000,
                                     0xff00000000, 0xff0000000000, 0xff000000000000, 0xff00000000000000};

static std::unordered_map<Piece, char> pieceToChar({{WhitePawn, 'P'},
                                                    {WhiteKnight, 'N'},
                                                    {WhiteBishop, 'B'},
                                                    {WhiteRook, 'R'},
                                                    {WhiteQueen, 'Q'},
                                                    {WhiteKing, 'K'},
                                                    {BlackPawn, 'p'},
                                                    {BlackKnight, 'n'},
                                                    {BlackBishop, 'b'},
                                                    {BlackRook, 'r'},
                                                    {BlackQueen, 'q'},
                                                    {BlackKing, 'k'},
                                                    {None, '.'}});

static std::unordered_map<char, Piece> charToPiece({{'P', WhitePawn},
                                                    {'N', WhiteKnight},
                                                    {'B', WhiteBishop},
                                                    {'R', WhiteRook},
                                                    {'Q', WhiteQueen},
                                                    {'K', WhiteKing},
                                                    {'p', BlackPawn},
                                                    {'n', BlackKnight},
                                                    {'b', BlackBishop},
                                                    {'r', BlackRook},
                                                    {'q', BlackQueen},
                                                    {'k', BlackKing},
                                                    {'.', None}});

static std::unordered_map<PieceType, char> PieceTypeToPromPiece(
    {{KNIGHT, 'n'}, {BISHOP, 'b'}, {ROOK, 'r'}, {QUEEN, 'q'}});

static std::unordered_map<char, PieceType> pieceToInt(
    {{'n', KNIGHT}, {'b', BISHOP}, {'r', ROOK}, {'q', QUEEN}, {'N', KNIGHT}, {'B', BISHOP}, {'R', ROOK}, {'Q', QUEEN}});

static std::unordered_map<Square, CastlingRight> castlingMapRook({{SQ_A1, wq}, {SQ_H1, wk}, {SQ_A8, bq}, {SQ_H8, bk}});

static std::unordered_map<char, CastlingRight> readCastleString({{'K', wk}, {'k', bk}, {'Q', wq}, {'q', bq}});

// *******************
// Move encoding
// *******************

constexpr inline Square from(Move move)
{
    return Square(move & 0b111111);
}

constexpr inline Square to(Move move)
{
    return Square((move & 0b111111000000) >> 6);
}

constexpr inline PieceType piece(Move move)
{
    return PieceType((move & 0b111000000000000) >> 12);
}

constexpr inline bool promoted(Move move)
{
    return bool((move & 0b1000000000000000) >> 15);
}

constexpr inline Move make(PieceType piece = NONETYPE, Square source = NO_SQ, Square target = NO_SQ,
                           bool promoted = false)
{
    return Move((uint16_t)source | (uint16_t)target << 6 | (uint16_t)piece << 12 | (uint16_t)promoted << 15);
}

template <PieceType piece, bool promoted> Move make(Square source = NO_SQ, Square target = NO_SQ)
{
    return Move((uint16_t)source | (uint16_t)target << 6 | (uint16_t)piece << 12 | (uint16_t)promoted << 15);
}

struct State
{
    Square enPassant{};
    uint8_t castling{};
    uint8_t halfMove{};
    Piece capturedPiece = None;
    State(Square enpassantCopy = {}, uint8_t castlingRightsCopy = {}, uint8_t halfMoveCopy = {},
          Piece capturedPieceCopy = None)
        : enPassant(enpassantCopy), castling(castlingRightsCopy), halfMove(halfMoveCopy),
          capturedPiece(capturedPieceCopy)
    {
    }
};

struct ExtMove
{
    int value;
    Move move;
};

inline constexpr bool operator==(const ExtMove &a, const ExtMove &b)
{
    return a.move == b.move;
}

inline constexpr bool operator>(const ExtMove &a, const ExtMove &b)
{
    return a.value > b.value;
}

inline constexpr bool operator<(const ExtMove &a, const ExtMove &b)
{
    return a.value < b.value;
}

struct Movelist
{
    ExtMove list[MAX_MOVES];
    uint8_t size = 0;
    typedef ExtMove *iterator;
    typedef const ExtMove *const_iterator;

    inline void Add(Move move)
    {
        list[size].move = move;
        list[size].value = 0;
        size++;
    }

    inline constexpr ExtMove &operator[](int i)
    {
        return list[i];
    }

    /// @brief
    /// @param m
    /// @return -1 if move was not found
    inline constexpr int find(Move m)
    {
        for (int i = 0; i < size; i++)
        {
            if (list[i].move == m)
                return i;
        }
        return -1;
    }

    inline iterator begin()
    {
        return (std::begin(list));
    }
    inline const_iterator begin() const
    {
        return (std::begin(list));
    }
    inline iterator end()
    {
        auto it = std::begin(list);
        std::advance(it, size);
        return it;
    }
    inline const_iterator end() const
    {
        auto it = std::begin(list);
        std::advance(it, size);
        return it;
    }
};

// *******************
// INTRINSIC FUNCTIONS
// *******************

// Compiler specific functions, taken from Stockfish https://github.com/official-stockfish/Stockfish
#if defined(__GNUC__) // GCC, Clang, ICC

inline Square lsb(U64 b)
{
    if (!b)
        return NO_SQ;
    return Square(__builtin_ctzll(b));
}

inline Square msb(U64 b)
{
    if (!b)
        return NO_SQ;
    return Square(63 ^ __builtin_clzll(b));
}

#elif defined(_MSC_VER) // MSVC

#ifdef _WIN64 // MSVC, WIN64
#include <intrin.h>
inline Square lsb(U64 b)
{
    unsigned long idx;
    _BitScanForward64(&idx, b);
    return (Square)idx;
}

inline Square msb(U64 b)
{
    unsigned long idx;
    _BitScanReverse64(&idx, b);
    return (Square)idx;
}

#endif

#else

#error "Compiler not supported."

#endif

inline uint8_t popcount(U64 mask)
{
#if defined(_MSC_VER) || defined(__INTEL_COMPILER)

    return (uint8_t)_mm_popcnt_u64(mask);

#else

    return __builtin_popcountll(mask);

#endif
}

/// @brief return the lsb and remove it
/// @param mask
/// @return
inline Square poplsb(U64 &mask)
{
    int8_t s = lsb(mask);
    mask &= mask - 1;
    return Square(s);
}

/// @brief splits a string into multiple parts, delimiter is whitespace
/// @param fen
/// @return std::vector
inline std::vector<std::string> splitInput(const std::string &input)
{
    std::stringstream input_stream(input);
    std::string segment;
    std::vector<std::string> seglist;

    while (std::getline(input_stream, segment, ' '))
    {
        seglist.push_back(segment);
    }

    return seglist;
}

/// @brief Gets the file index of the square where 0 is the a-file
/// @param sq
/// @return the file of the square
inline constexpr File square_file(Square sq)
{
    return File(sq & 7);
}

/// @brief Gets the rank index of the square where 0 is the first rank.
/// @param sq
/// @return the rank of the square
inline constexpr Rank square_rank(Square sq)
{
    return Rank(sq >> 3);
}

/// @brief makes a square out of rank and file
/// @param f
/// @param r
/// @return
inline constexpr Square file_rank_square(File f, Rank r)
{
    return Square((r << 3) + f);
}

/// @brief distance between two squares
/// @param a
/// @param b
/// @return
inline uint8_t square_distance(Square a, Square b)
{
    return std::max(std::abs(square_file(a) - square_file(b)), std::abs(square_rank(a) - square_rank(b)));
}

inline uint8_t diagonal_of(Square sq)
{
    return 7 + square_rank(sq) - square_file(sq);
}

inline uint8_t anti_diagonal_of(Square sq)
{
    return square_rank(sq) + square_file(sq);
}

/// @brief manhatten distance between two squares
/// @param sq1
/// @param sq2
/// @return
inline uint8_t manhatten_distance(Square sq1, Square sq2)
{
    return std::abs(square_file(sq1) - square_file(sq2)) + std::abs(square_rank(sq1) - square_rank(sq2));
}

/// @brief color of a square, has nothing to do with whose piece is on that square
/// @param square
/// @return
inline Color get_square_color(Square square)
{
    if ((square % 8) % 2 == (square / 8) % 2)
    {
        return Black;
    }
    else
    {
        return White;
    }
}

/// @brief get the piecetype of a piece
/// @param piece
/// @return the piecetype
inline PieceType type_of_piece(Piece piece)
{
    return PieceToPieceType[piece];
}

/// @brief makes a piece out of a PieceType and Color
/// @param type
/// @param c
/// @return
inline Piece makePiece(PieceType type, Color c)
{
    if (type == NONETYPE)
        return None;
    return Piece(type + 6 * c);
}

/// @brief prints any bitboard
/// @param bb
inline void printBitboard(U64 bb)
{
    std::bitset<64> b(bb);
    std::string str_bitset = b.to_string();
    for (int i = 0; i < MAX_SQ; i += 8)
    {
        std::string x = str_bitset.substr(i, 8);
        reverse(x.begin(), x.end());
        std::cout << x << std::endl;
    }
    std::cout << '\n' << std::endl;
}

inline U64 PawnAttacks(Square sq, Color c)
{
    return PAWN_ATTACKS_TABLE[c][sq];
}

inline U64 KnightAttacks(Square sq)
{
    return KNIGHT_ATTACKS_TABLE[sq];
}

inline U64 KingAttacks(Square sq)
{
    return KING_ATTACKS_TABLE[sq];
}

class Board
{
  public:
    Color sideToMove;

    // NO_SQ when enpassant is not possible
    Square enPassantSquare;

    // castling right is encoded in 8bit
    // wk = 1
    // wq = 2
    // bk = 4
    // bq = 8
    // 0  0  0  0  0  0  0  0
    // wk wq bk bq
    uint8_t castlingRights;

    // halfmoves start at 0
    uint8_t halfMoveClock;

    // full moves start at 1
    uint16_t fullMoveNumber;

    // current hashkey
    U64 hashKey;

    U64 piecesBB[12] = {};
    Piece board[MAX_SQ];

    // keeps track on how many checks there currently are
    // 2 = only king moves are valid
    // 1 = king move, block/capture
    uint8_t doubleCheck;

    // the path between horizontal/vertical pinners and
    // the pinned is set
    U64 pinHV;

    // the path between diagonal pinners and
    // the pinned is set
    U64 pinD;

    // all bits set if we are not in check
    // otherwise the path between the king and the checker
    // in case of knights giving check only the knight square
    // is checked
    U64 checkMask = DEFAULT_CHECKMASK;

    // all squares that are seen by an enemy piece
    U64 seen;

    // Occupation piecesBB
    U64 occEnemy;
    U64 occUs;
    U64 occAll;
    U64 enemyEmptyBB;

    U64 SQUARES_BETWEEN_BB[MAX_SQ][MAX_SQ];

  private:
    // keeps track of previous hashes, used for
    // repetition detection
    std::vector<U64> hashHistory;

    std::vector<State> stateHistory;

  public:
    /// @brief constructor for the board, loads startpos and initializes SQUARES_BETWEEN_BB array
    Board(std::string fen = DEFAULT_POS);

    /// @brief Finds what piece is on the square using bitboards (slow)
    /// @param sq
    /// @return found piece otherwise None
    Piece pieceAtBB(Square sq);

    /// @brief Finds what piecetype is on the square
    /// @param sq
    /// @return found piece otherwise None
    PieceType pieceTypeAtB(Square sq) const;

    /// @brief Finds what piece is on the square using the board (more performant)
    /// @param sq
    /// @return found piece otherwise None
    Piece pieceAtB(Square sq) const;

    /// @brief applys a new Fen to the board
    /// @param fen
    void applyFen(const std::string &fen);

    /// @brief returns a Fen string of the current board
    /// @return fen string
    std::string getFen() const;

    /// @brief detects if the position is a repetition by default 1, fide would be 2
    /// @param draw
    /// @return true for repetition otherwise false
    bool isRepetition(int draw = 2) const;

    /// @brief false if only pawns on the board
    bool nonPawnMat(Color c) const;

    Square KingSQ(Color c) const;

    U64 Enemy(Color c) const;
    U64 EnemyEmpty(Color c) const;
    U64 Us(Color c) const;

    template <Color c> U64 Us() const
    {
        return piecesBB[PAWN + c * 6] | piecesBB[KNIGHT + c * 6] | piecesBB[BISHOP + c * 6] | piecesBB[ROOK + c * 6] |
               piecesBB[QUEEN + c * 6] | piecesBB[KING + c * 6];
    }

    inline U64 All() const
    {
        return Us<White>() | Us<Black>();
    }

    // Gets individual piece bitboards

    template <Piece p> constexpr U64 pieces() const
    {
        return piecesBB[p];
    }

    template <PieceType p, Color c> constexpr U64 pieces() const
    {
        return piecesBB[p + c * 6];
    }

    inline constexpr U64 pieces(PieceType p, Color c) const
    {
        return piecesBB[p + c * 6];
    }

    /// @brief returns the color of a piece at a square
    /// @param loc
    /// @return
    Color colorOf(Square loc) const;

    /// @brief is square attacked by color c
    /// @param c
    /// @param sq
    /// @return
    bool isSquareAttacked(Color c, Square sq) const;

    /// @brief plays the move on the internal board
    /// @param move
    void makeMove(Move move);

    /// @brief unmake a move played on the internal board
    /// @param move
    void unmakeMove(Move move);

    /// @brief make a nullmove
    void makeNullMove();

    /// @brief unmake a nullmove
    void unmakeNullMove();

    /// @brief Remove a Piece from the board
    /// @param piece
    /// @param sq
    void removePiece(Piece piece, Square sq);

    /// @brief Place a Piece on the board
    /// @param piece
    /// @param sq
    void placePiece(Piece piece, Square sq);

    /// @brief Move a piece on the board
    /// @param piece
    /// @param fromSq
    /// @param toSq
    void movePiece(Piece piece, Square fromSq, Square toSq);

    U64 attacksByPiece(PieceType pt, Square sq, Color c) const;

    friend std::ostream &operator<<(std::ostream &os, const Board &b);

  private:
    /// @brief calculate the current zobrist hash from scratch
    /// @return
    U64 zobristHash() const;

    /// @brief initialize SQUARES_BETWEEN_BB array
    void initializeLookupTables();

    // update the hash

    U64 updateKeyPiece(Piece piece, Square sq) const;
    U64 updateKeyCastling() const;
    U64 updateKeyEnPassant(Square sq) const;
    U64 updateKeySideToMove() const;

    void removeCastlingRightsAll(Color c);
    void removeCastlingRightsRook(Square sq);
};

inline Board::Board(std::string fen)
{
    initializeLookupTables();
    stateHistory.reserve(MAX_PLY);
    hashHistory.reserve(512);

    sideToMove = White;
    enPassantSquare = NO_SQ;
    castlingRights = wk | wq | bk | bq;
    halfMoveClock = 0;
    fullMoveNumber = 1;

    pinHV = 0;
    pinD = 0;
    doubleCheck = 0;
    checkMask = DEFAULT_CHECKMASK;
    seen = 0;

    std::fill(std::begin(board), std::end(board), None);

    applyFen(fen);

    occEnemy = Enemy(sideToMove);
    occUs = Us(sideToMove);
    occAll = All();
    enemyEmptyBB = EnemyEmpty(sideToMove);
}

inline Piece Board::pieceAtBB(Square sq)
{
    for (Piece p = WhitePawn; p < None; p++)
    {
        if (piecesBB[p] & (1ULL << sq))
            return p;
    }
    return None;
}

inline Piece Board::pieceAtB(Square sq) const
{
    return board[sq];
}

inline PieceType Board::pieceTypeAtB(Square sq) const
{
    return type_of_piece(board[sq]);
}

inline void Board::applyFen(const std::string &fen)
{
    for (Piece p = WhitePawn; p < None; p++)
    {
        piecesBB[p] = 0ULL;
    }

    const std::vector<std::string> params = splitInput(fen);

    const std::string position = params[0];
    const std::string move_right = params[1];
    const std::string castling = params[2];
    const std::string en_passant = params[3];

    const std::string half_move_clock = params.size() > 4 ? params[4] : "0";
    const std::string full_move_counter = params.size() > 4 ? params[5] : "1";

    sideToMove = (move_right == "w") ? White : Black;

    std::fill(std::begin(board), std::end(board), None);

    Square square = Square(56);
    for (int index = 0; index < static_cast<int>(position.size()); index++)
    {
        char curr = position[index];
        if (charToPiece.find(curr) != charToPiece.end())
        {
            const Piece piece = charToPiece[curr];
            placePiece(piece, square);

            square = Square(square + 1);
        }
        else if (curr == '/')
            square = Square(square - 16);
        else if (isdigit(curr))
            square = Square(square + (curr - '0'));
    }

    removeCastlingRightsAll(White);
    removeCastlingRightsAll(Black);

    for (size_t i = 0; i < castling.size(); i++)
    {
        if (readCastleString.find(castling[i]) != readCastleString.end())
            castlingRights |= readCastleString[castling[i]];
    }

    if (en_passant == "-")
    {
        enPassantSquare = NO_SQ;
    }
    else
    {
        char letter = en_passant[0];
        int file = letter - 96;
        int rank = en_passant[1] - 48;
        enPassantSquare = Square((rank - 1) * 8 + file - 1);
    }

    halfMoveClock = std::stoi(half_move_clock);

    // full_move_counter actually half moves
    fullMoveNumber = std::stoi(full_move_counter) * 2;

    hashKey = zobristHash();

    hashHistory.clear();
    stateHistory.clear();

    hashHistory.push_back(hashKey);
}

inline std::string Board::getFen() const
{
    std::stringstream ss;

    // Loop through the ranks of the board in reverse order
    for (int rank = 7; rank >= 0; rank--)
    {
        int free_space = 0;

        // Loop through the files of the board
        for (int file = 0; file < 8; file++)
        {
            // Calculate the square index
            int sq = rank * 8 + file;

            // Get the piece at the current square
            Piece piece = pieceAtB(Square(sq));

            // If there is a piece at the current square
            if (piece != None)
            {
                // If there were any empty squares before this piece,
                // append the number of empty squares to the FEN string
                if (free_space)
                {
                    ss << free_space;
                    free_space = 0;
                }

                // Append the character representing the piece to the FEN string
                ss << pieceToChar[piece];
            }
            else
            {
                // If there is no piece at the current square, increment the
                // counter for the number of empty squares
                free_space++;
            }
        }

        // If there are any empty squares at the end of the rank,
        // append the number of empty squares to the FEN string
        if (free_space != 0)
        {
            ss << free_space;
        }

        // Append a "/" character to the FEN string, unless this is the last rank
        ss << (rank > 0 ? "/" : "");
    }

    // Append " w " or " b " to the FEN string, depending on which player's turn it is
    ss << (sideToMove == White ? " w " : " b ");

    // Append the appropriate characters to the FEN string to indicate
    // whether or not castling is allowed for each player
    if (castlingRights & wk)
        ss << "K";
    if (castlingRights & wq)
        ss << "Q";
    if (castlingRights & bk)
        ss << "k";
    if (castlingRights & bq)
        ss << "q";
    if (castlingRights == 0)
        ss << "-";

    // Append information about the en passant square (if any)
    // and the halfmove clock and fullmove number to the FEN string
    if (enPassantSquare == NO_SQ)
        ss << " - ";
    else
        ss << " " << squareToString[enPassantSquare] << " ";

    ss << int(halfMoveClock) << " " << int(fullMoveNumber / 2);

    // Return the resulting FEN string
    return ss.str();
}

inline bool Board::isRepetition(int draw) const
{
    uint8_t c = 0;

    for (int i = static_cast<int>(hashHistory.size()) - 2;
         i >= 0 && i >= static_cast<int>(hashHistory.size()) - halfMoveClock - 1; i -= 2)
    {
        if (hashHistory[i] == hashKey)
            c++;
        if (c == draw)
            return true;
    }

    return false;
}

inline bool Board::nonPawnMat(Color c) const
{
    return pieces(KNIGHT, c) | pieces(BISHOP, c) | pieces(ROOK, c) | pieces(QUEEN, c);
}

inline Square Board::KingSQ(Color c) const
{
    return lsb(pieces(KING, c));
}

inline U64 Board::Enemy(Color c) const
{
    return Us(~c);
}

inline U64 Board::Us(Color c) const
{
    return piecesBB[PAWN + c * 6] | piecesBB[KNIGHT + c * 6] | piecesBB[BISHOP + c * 6] | piecesBB[ROOK + c * 6] |
           piecesBB[QUEEN + c * 6] | piecesBB[KING + c * 6];
}

inline U64 Board::EnemyEmpty(Color c) const
{
    return ~Us(c);
}

inline Color Board::colorOf(Square loc) const
{
    return Color((pieceAtB(loc) / 6));
}

inline bool Board::isSquareAttacked(Color c, Square sq) const
{
    if (pieces(PAWN, c) & PawnAttacks(sq, ~c))
        return true;
    if (pieces(KNIGHT, c) & KnightAttacks(sq))
        return true;

    const U64 bb = All();

    if ((pieces(BISHOP, c) | pieces(QUEEN, c)) & BishopAttacks(sq, bb))
        return true;
    if ((pieces(ROOK, c) | pieces(QUEEN, c)) & RookAttacks(sq, bb))
        return true;
    if (pieces(KING, c) & KingAttacks(sq))
        return true;
    return false;
}

inline void Board::makeMove(Move move)
{
    PieceType pt = piece(move);
    Piece p = makePiece(pt, sideToMove);
    Square from_sq = from(move);
    Square to_sq = to(move);
    Piece capture = board[to_sq];

    assert(from_sq >= 0 && from_sq < MAX_SQ);
    assert(to_sq >= 0 && to_sq < MAX_SQ);
    assert(type_of_piece(capture) != KING);
    assert(p != None);
    assert((promoted(move) && (pt != PAWN && pt != KING)) || !promoted(move));

    // *****************************
    // STORE STATE HISTORY
    // *****************************

    hashHistory.emplace_back(hashKey);
    stateHistory.emplace_back(State(enPassantSquare, castlingRights, halfMoveClock, capture));

    halfMoveClock++;
    fullMoveNumber++;

    bool ep = to_sq == enPassantSquare;
    const bool isCastlingWhite =
        (p == WhiteKing && capture == WhiteRook) || (p == WhiteKing && square_distance(to_sq, from_sq) >= 2);
    const bool isCastlingBlack =
        (p == BlackKing && capture == BlackRook) || (p == BlackKing && square_distance(to_sq, from_sq) >= 2);

    // *****************************
    // UPDATE HASH
    // *****************************

    if (enPassantSquare != NO_SQ)
        hashKey ^= updateKeyEnPassant(enPassantSquare);
    enPassantSquare = NO_SQ;

    hashKey ^= updateKeyCastling();

    if (isCastlingWhite || isCastlingBlack)
    {
        Piece rook = sideToMove == White ? WhiteRook : BlackRook;
        Square rookSQ = file_rank_square(to_sq > from_sq ? FILE_F : FILE_D, square_rank(from_sq));

        assert(type_of_piece(pieceAtB(to_sq)) == ROOK);
        hashKey ^= updateKeyPiece(rook, to_sq);
        hashKey ^= updateKeyPiece(rook, rookSQ);
    }

    if (pt == KING)
    {
        removeCastlingRightsAll(sideToMove);
    }
    else if (pt == ROOK)
    {
        removeCastlingRightsRook(from_sq);
    }
    else if (pt == PAWN)
    {
        halfMoveClock = 0;
        if (ep)
        {
            hashKey ^= updateKeyPiece(makePiece(PAWN, ~sideToMove), Square(to_sq - (sideToMove * -2 + 1) * 8));
        }
        else if (std::abs(from_sq - to_sq) == 16)
        {
            U64 epMask = PawnAttacks(Square(to_sq - (sideToMove * -2 + 1) * 8), sideToMove);
            if (epMask & pieces(PAWN, ~sideToMove))
            {
                enPassantSquare = Square(to_sq - (sideToMove * -2 + 1) * 8);
                hashKey ^= updateKeyEnPassant(enPassantSquare);

                assert(pieceAtB(enPassantSquare) == None);
            }
        }
    }

    if (capture != None && !(isCastlingWhite || isCastlingBlack))
    {
        halfMoveClock = 0;
        hashKey ^= updateKeyPiece(capture, to_sq);
        if (type_of_piece(capture) == ROOK)
            removeCastlingRightsRook(to_sq);
    }

    if (promoted(move))
    {
        halfMoveClock = 0;

        hashKey ^= updateKeyPiece(makePiece(PAWN, sideToMove), from_sq);
        hashKey ^= updateKeyPiece(p, to_sq);
    }
    else
    {
        hashKey ^= updateKeyPiece(p, from_sq);
        hashKey ^= updateKeyPiece(p, to_sq);
    }

    hashKey ^= updateKeySideToMove();
    hashKey ^= updateKeyCastling();

    // *****************************
    // UPDATE PIECES
    // *****************************

    if (isCastlingWhite || isCastlingBlack)
    {
        Square rookToSq;
        Piece rook = sideToMove == White ? WhiteRook : BlackRook;

        removePiece(p, from_sq);
        removePiece(rook, to_sq);

        rookToSq = file_rank_square(to_sq > from_sq ? FILE_F : FILE_D, square_rank(from_sq));
        to_sq = file_rank_square(to_sq > from_sq ? FILE_G : FILE_C, square_rank(from_sq));

        placePiece(p, to_sq);
        placePiece(rook, rookToSq);
    }
    else if (pt == PAWN && ep)
    {
        assert(pieceAtB(Square(to_sq - (sideToMove * -2 + 1) * 8)) != None);

        removePiece(makePiece(PAWN, ~sideToMove), Square(to_sq - (sideToMove * -2 + 1) * 8));
    }
    else if (capture != None && !(isCastlingWhite || isCastlingBlack))
    {
        assert(pieceAtB(to_sq) != None);

        removePiece(capture, to_sq);
    }

    if (promoted(move))
    {
        assert(pieceAtB(to_sq) == None);

        removePiece(makePiece(PAWN, sideToMove), from_sq);
        placePiece(p, to_sq);
    }
    else if (!(isCastlingWhite || isCastlingBlack))
    {
        assert(pieceAtB(to_sq) == None);

        movePiece(p, from_sq, to_sq);
    }

    sideToMove = ~sideToMove;
}

inline void Board::unmakeMove(Move move)
{
    const State restore = stateHistory.back();
    stateHistory.pop_back();

    hashKey = hashHistory.back();
    hashHistory.pop_back();

    enPassantSquare = restore.enPassant;
    castlingRights = restore.castling;
    halfMoveClock = restore.halfMove;
    Piece capture = restore.capturedPiece;

    fullMoveNumber--;

    Square from_sq = from(move);
    Square to_sq = to(move);
    bool promotion = promoted(move);

    sideToMove = ~sideToMove;
    PieceType pt = piece(move);
    Piece p = makePiece(pt, sideToMove);

    const bool isCastlingWhite =
        (p == WhiteKing && capture == WhiteRook) || (p == WhiteKing && square_distance(to_sq, from_sq) >= 2);
    const bool isCastlingBlack =
        (p == BlackKing && capture == BlackRook) || (p == BlackKing && square_distance(to_sq, from_sq) >= 2);

    if (isCastlingWhite || isCastlingBlack)
    {
        Square rookToSq = to_sq;
        Piece rook = sideToMove == White ? WhiteRook : BlackRook;
        Square rookFromSq = file_rank_square(to_sq > from_sq ? FILE_F : FILE_D, square_rank(from_sq));
        to_sq = file_rank_square(to_sq > from_sq ? FILE_G : FILE_C, square_rank(from_sq));

        // We need to remove both pieces first and then place them back.
        removePiece(rook, rookFromSq);
        removePiece(p, to_sq);

        placePiece(p, from_sq);
        placePiece(rook, rookToSq);
    }
    else if (promotion)
    {
        removePiece(p, to_sq);
        placePiece(makePiece(PAWN, sideToMove), from_sq);
        if (capture != None)
            placePiece(capture, to_sq);
        return;
    }
    else
    {
        movePiece(p, to_sq, from_sq);
    }

    if (to_sq == enPassantSquare && pt == PAWN)
    {
        int8_t offset = sideToMove == White ? -8 : 8;
        placePiece(makePiece(PAWN, ~sideToMove), Square(enPassantSquare + offset));
    }
    else if (capture != None && !(isCastlingWhite || isCastlingBlack))
    {
        placePiece(capture, to_sq);
    }
}

inline void Board::makeNullMove()
{
    stateHistory.emplace_back(State(enPassantSquare, castlingRights, halfMoveClock, None));
    sideToMove = ~sideToMove;

    hashKey ^= updateKeySideToMove();
    if (enPassantSquare != NO_SQ)
        hashKey ^= updateKeyEnPassant(enPassantSquare);

    enPassantSquare = NO_SQ;
    fullMoveNumber++;
}

inline void Board::unmakeNullMove()
{
    const State restore = stateHistory.back();
    stateHistory.pop_back();

    enPassantSquare = restore.enPassant;
    castlingRights = restore.castling;
    halfMoveClock = restore.halfMove;

    hashKey ^= updateKeySideToMove();
    if (enPassantSquare != NO_SQ)
        hashKey ^= updateKeyEnPassant(enPassantSquare);

    fullMoveNumber--;
    sideToMove = ~sideToMove;
}

inline void Board::removePiece(Piece piece, Square sq)
{
    piecesBB[piece] &= ~(1ULL << sq);
    board[sq] = None;
}

inline void Board::placePiece(Piece piece, Square sq)
{
    piecesBB[piece] |= (1ULL << sq);
    board[sq] = piece;
}

inline void Board::movePiece(Piece piece, Square fromSq, Square toSq)
{
    piecesBB[piece] &= ~(1ULL << fromSq);
    piecesBB[piece] |= (1ULL << toSq);
    board[fromSq] = None;
    board[toSq] = piece;
}

inline U64 Board::attacksByPiece(PieceType pt, Square sq, Color c) const
{
    switch (pt)
    {
    case PAWN:
        return PawnAttacks(sq, c);
    case KNIGHT:
        return KnightAttacks(sq);
    case BISHOP:
        return BishopAttacks(sq, All());
    case ROOK:
        return RookAttacks(sq, All());
    case QUEEN:
        return QueenAttacks(sq, All());
    case KING:
        return KingAttacks(sq);
    case NONETYPE:
        return 0ULL;
    default:
        return 0ULL;
    }
}

std::ostream &operator<<(std::ostream &os, const Board &b)
{
    for (int i = 63; i >= 0; i -= 8)
    {
        os << " " << pieceToChar[b.board[i - 7]] << " " << pieceToChar[b.board[i - 6]] << " "
           << pieceToChar[b.board[i - 5]] << " " << pieceToChar[b.board[i - 4]] << " " << pieceToChar[b.board[i - 3]]
           << " " << pieceToChar[b.board[i - 2]] << " " << pieceToChar[b.board[i - 1]] << " " << pieceToChar[b.board[i]]
           << " \n";
    }
    os << "\n\n";
    os << "Fen: " << b.getFen() << "\n";
    os << "Side to move: " << static_cast<int>(b.sideToMove) << "\n";
    os << "Castling rights: " << static_cast<int>(b.castlingRights) << "\n";
    os << "Halfmoves: " << static_cast<int>(b.halfMoveClock) << "\n";
    os << "Fullmoves: " << static_cast<int>(b.fullMoveNumber) / 2 << "\n";
    os << "EP: " << static_cast<int>(b.enPassantSquare) << "\n";
    os << "Hash: " << b.hashKey << "\n";

    os << std::endl;
    return os;
}

// ***************************
// PRIVATE BOARD FUNCTION DECLARTION
// ***************************

inline U64 Board::zobristHash() const
{
    U64 hash = 0ULL;
    U64 wPieces = Us(White);
    U64 bPieces = Us(Black);

    // Piece hashes
    while (wPieces)
    {
        Square sq = poplsb(wPieces);
        hash ^= updateKeyPiece(pieceAtB(sq), sq);
    }
    while (bPieces)
    {
        Square sq = poplsb(bPieces);
        hash ^= updateKeyPiece(pieceAtB(sq), sq);
    }

    U64 ep_hash = 0ULL;

    if (enPassantSquare != NO_SQ)
        ep_hash = updateKeyEnPassant(enPassantSquare);

    U64 turn_hash = sideToMove == White ? RANDOM_ARRAY[780] : 0;

    U64 cast_hash = updateKeyCastling();

    return hash ^ cast_hash ^ turn_hash ^ ep_hash;
}

inline void Board::initializeLookupTables()
{
    // initialize squares between table
    U64 sqs;
    for (Square sq1 = SQ_A1; sq1 <= SQ_H8; ++sq1)
    {
        for (Square sq2 = SQ_A1; sq2 <= SQ_H8; ++sq2)
        {
            sqs = (1ULL << sq1) | (1ULL << sq2);
            if (sq1 == sq2)
                SQUARES_BETWEEN_BB[sq1][sq2] = 0ull;
            else if (square_file(sq1) == square_file(sq2) || square_rank(sq1) == square_rank(sq2))
                SQUARES_BETWEEN_BB[sq1][sq2] = RookAttacks(sq1, sqs) & RookAttacks(sq2, sqs);
            else if (diagonal_of(sq1) == diagonal_of(sq2) || anti_diagonal_of(sq1) == anti_diagonal_of(sq2))
                SQUARES_BETWEEN_BB[sq1][sq2] = BishopAttacks(sq1, sqs) & BishopAttacks(sq2, sqs);
        }
    }
}

inline U64 Board::updateKeyPiece(Piece piece, Square sq) const
{
    return RANDOM_ARRAY[64 * hash_piece[piece] + sq];
}

inline U64 Board::updateKeyEnPassant(Square sq) const
{
    return RANDOM_ARRAY[772 + square_file(sq)];
}

inline U64 Board::updateKeyCastling() const
{
    return castlingKey[castlingRights];
}

inline U64 Board::updateKeySideToMove() const
{
    return RANDOM_ARRAY[780];
}

inline void Board::removeCastlingRightsAll(Color c)
{
    if (c == White)
    {
        castlingRights &= ~(wk | wq);
    }
    else if (c == Black)
    {
        castlingRights &= ~(bk | bq);
    }
}

inline void Board::removeCastlingRightsRook(Square sq)
{
    if (castlingMapRook.find(sq) != castlingMapRook.end())
    {
        castlingRights &= ~castlingMapRook[sq];
    }
}

/// @brief uci representation of a move
/// @param move
/// @return
inline std::string convertMoveToUci(Move move)
{
    std::stringstream ss;

    // Get the from and to squares
    Square from_sq = from(move);
    Square to_sq = to(move);

    // Add the from and to squares to the string stream
    ss << squareToString[from_sq];
    ss << squareToString[to_sq];

    // If the move is a promotion, add the promoted piece to the string stream
    if (promoted(move))
    {
        ss << PieceTypeToPromPiece[piece(move)];
    }

    return ss.str();
}

/// @brief helper function
/// @param squareStr
/// @return
inline Square extractSquare(std::string_view squareStr)
{
    char letter = squareStr[0];
    int file = letter - 96;
    int rank = squareStr[1] - 48;
    int index = (rank - 1) * 8 + file - 1;
    return Square(index);
}

/// @brief Convert a string move to an internal move
/// @param board
/// @param input
/// @return
inline Move convertUciToMove(const Board &board, const std::string &input)
{
    Square source = extractSquare(input.substr(0, 2));
    Square target = extractSquare(input.substr(2, 2));
    PieceType piece = board.pieceTypeAtB(source);

    // convert to king captures rook
    if (piece == KING && square_distance(target, source) == 2)
    {
        target = file_rank_square(target > source ? FILE_H : FILE_A, square_rank(source));
    }

    switch (input.length())
    {
    case 4:
        return make(piece, source, target, false);
    case 5:
        return make(pieceToInt[input.at(4)], source, target, true);
    default:
        std::cout << "FALSE INPUT" << std::endl;
        return make(NONETYPE, NO_SQ, NO_SQ, false);
    }
}

} // namespace Chess

namespace Movegen
{
using namespace Chess;

template <Color c> U64 pawnLeftAttacks(const U64 pawns)
{
    return c == White ? (pawns << 7) & ~MASK_FILE[FILE_H] : (pawns >> 7) & ~MASK_FILE[FILE_A];
}

template <Color c> U64 pawnRightAttacks(const U64 pawns)
{
    return c == White ? (pawns << 9) & ~MASK_FILE[FILE_A] : (pawns >> 9) & ~MASK_FILE[FILE_H];
}

/********************
 * Creates the checkmask.
 * A checkmask is the path from the enemy checker to our king.
 * Knight and pawns get themselves added to the checkmask, otherwise the path is added.
 * When there is no check at all all bits are set (DEFAULT_CHECKMASK)
 *******************/
template <Color c> U64 DoCheckmask(Board &board, Square sq)
{
    U64 Occ = board.occAll;
    U64 checks = 0ULL;
    U64 pawn_mask = board.pieces<PAWN, ~c>() & PawnAttacks(sq, c);
    U64 knight_mask = board.pieces<KNIGHT, ~c>() & KnightAttacks(sq);
    U64 bishop_mask = (board.pieces<BISHOP, ~c>() | board.pieces<QUEEN, ~c>()) & BishopAttacks(sq, Occ);
    U64 rook_mask = (board.pieces<ROOK, ~c>() | board.pieces<QUEEN, ~c>()) & RookAttacks(sq, Occ);

    /********************
     * We keep track of the amount of checks, in case there are
     * two checks on the board only the king can move!
     *******************/
    board.doubleCheck = 0;

    if (pawn_mask)
    {
        checks |= pawn_mask;
        board.doubleCheck++;
    }
    if (knight_mask)
    {
        checks |= knight_mask;
        board.doubleCheck++;
    }
    if (bishop_mask)
    {
        int8_t index = lsb(bishop_mask);

        // Now we add the path!
        checks |= board.SQUARES_BETWEEN_BB[sq][index] | (1ULL << index);
        board.doubleCheck++;
    }
    if (rook_mask)
    {
        /********************
         * 3nk3/4P3/8/8/8/8/8/2K1R3 w - - 0 1, pawn promotes to queen or rook and
         * suddenly the same piecetype gives check two times
         * in that case we have a double check and can return early
         * because king moves dont require the checkmask.
         *******************/
        if (popcount(rook_mask) > 1)
        {
            board.doubleCheck = 2;
            return checks;
        }

        int8_t index = lsb(rook_mask);

        // Now we add the path!
        checks |= board.SQUARES_BETWEEN_BB[sq][index] | (1ULL << index);
        board.doubleCheck++;
    }

    if (!checks)
        return DEFAULT_CHECKMASK;

    return checks;
}

/********************
 * Create the pinmask
 * We define the pin mask as the path from the enemy pinner
 * through the pinned piece to our king.
 * We assume that the king can do rook and bishop moves and generate the attacks
 * for these in case we hit an enemy rook/bishop/queen we might have a possible pin.
 * We need to confirm the pin because there could be 2 or more pieces from our king to
 * the possible pinner. We do this by simply using the popcount
 * of our pieces that lay on the pin mask, if it is only 1 piece then that piece is pinned.
 *******************/
template <Color c> U64 DoPinMaskRooks(Board &board, Square sq)
{
    U64 rook_mask = (board.pieces<ROOK, ~c>() | board.pieces<QUEEN, ~c>()) & RookAttacks(sq, board.occEnemy);

    U64 pinHV = 0ULL;
    while (rook_mask)
    {
        const Square index = poplsb(rook_mask);
        const U64 possible_pin = (board.SQUARES_BETWEEN_BB[sq][index] | (1ULL << index));
        if (popcount(possible_pin & board.occUs) == 1)
            pinHV |= possible_pin;
    }
    return pinHV;
}

template <Color c> U64 DoPinMaskBishops(Board &board, Square sq)
{
    U64 bishop_mask = (board.pieces<BISHOP, ~c>() | board.pieces<QUEEN, ~c>()) & BishopAttacks(sq, board.occEnemy);

    U64 pinD = 0ULL;

    while (bishop_mask)
    {
        const Square index = poplsb(bishop_mask);
        const U64 possible_pin = (board.SQUARES_BETWEEN_BB[sq][index] | (1ULL << index));
        if (popcount(possible_pin & board.occUs) == 1)
            pinD |= possible_pin;
    }

    return pinD;
}

/********************
 * Seen squares
 * We keep track of all attacked squares by the enemy
 * this is used for king move generation.
 *******************/
template <Color c> U64 seenSquares(Board &board)
{
    const Square kSq = board.KingSQ(~c);

    U64 pawns = board.pieces<PAWN, c>();
    U64 knights = board.pieces<KNIGHT, c>();
    U64 queens = board.pieces<QUEEN, c>();
    U64 bishops = board.pieces<BISHOP, c>() | queens;
    U64 rooks = board.pieces<ROOK, c>() | queens;

    // Remove our king
    board.occAll &= ~(1ULL << kSq);

    U64 seen = pawnLeftAttacks<c>(pawns) | pawnRightAttacks<c>(pawns);

    while (knights)
    {
        Square index = poplsb(knights);
        seen |= KnightAttacks(index);
    }
    while (bishops)
    {
        Square index = poplsb(bishops);
        seen |= BishopAttacks(index, board.occAll);
    }
    while (rooks)
    {
        Square index = poplsb(rooks);
        seen |= RookAttacks(index, board.occAll);
    }

    Square index = lsb(board.pieces<KING, c>());
    seen |= KingAttacks(index);

    // Place our King back
    board.occAll |= (1ULL << kSq);

    return seen;
}

/********************
 * Creates the pinmask and checkmask
 * setup important variables that we use for move generation.
 *******************/
template <Color c> void init(Board &board, Square sq)
{
    board.occUs = board.Us<c>();
    board.occEnemy = board.Us<~c>();
    board.occAll = board.occUs | board.occEnemy;
    board.enemyEmptyBB = ~board.occUs;

    board.seen = seenSquares<~c>(board);
    board.checkMask = DoCheckmask<c>(board, sq);
    board.pinHV = DoPinMaskRooks<c>(board, sq);
    board.pinD = DoPinMaskBishops<c>(board, sq);
}

/// @brief shift a mask in a direction
/// @tparam direction
/// @param b
/// @return
template <Direction direction> constexpr U64 shift(const U64 b)
{
    switch (direction)
    {
    case NORTH:
        return b << 8;
    case SOUTH:
        return b >> 8;
    case NORTH_WEST:
        return (b & ~MASK_FILE[0]) << 7;
    case WEST:
        return (b & ~MASK_FILE[0]) >> 1;
    case SOUTH_WEST:
        return (b & ~MASK_FILE[0]) >> 9;
    case NORTH_EAST:
        return (b & ~MASK_FILE[7]) << 9;
    case EAST:
        return (b & ~MASK_FILE[7]) << 1;
    case SOUTH_EAST:
        return (b & ~MASK_FILE[7]) >> 7;
    }
}

// all legal moves for each piece

/// @brief all legal pawn moves, generated at once
/// @tparam c
/// @tparam mt
/// @param board
/// @param movelist
template <Color c, Movetype mt> void LegalPawnMovesAll(Board &board, Movelist &movelist)
{
    const U64 pawns_mask = board.pieces(PAWN, c);

    constexpr Direction UP = c == White ? NORTH : SOUTH;
    constexpr Direction DOWN = c == Black ? NORTH : SOUTH;
    constexpr Direction DOWN_LEFT = c == Black ? NORTH_EAST : SOUTH_WEST;
    constexpr Direction DOWN_RIGHT = c == Black ? NORTH_WEST : SOUTH_EAST;
    constexpr U64 RANK_BEFORE_PROMO = c == White ? MASK_RANK[RANK_7] : MASK_RANK[RANK_2];
    constexpr U64 RANK_PROMO = c == White ? MASK_RANK[RANK_8] : MASK_RANK[RANK_1];
    constexpr U64 doublePushRank = c == White ? MASK_RANK[RANK_3] : MASK_RANK[RANK_6];

    // These pawns can maybe take Left or Right
    const U64 pawnsLR = pawns_mask & ~board.pinHV;

    const U64 unpinnedpawnsLR = pawnsLR & ~board.pinD;
    const U64 pinnedpawnsLR = pawnsLR & board.pinD;

    U64 Lpawns = (pawnLeftAttacks<c>(unpinnedpawnsLR)) | (pawnLeftAttacks<c>(pinnedpawnsLR) & board.pinD);
    U64 Rpawns = (pawnRightAttacks<c>(unpinnedpawnsLR)) | (pawnRightAttacks<c>(pinnedpawnsLR) & board.pinD);

    // Prune moves that dont capture a piece and are not on the checkmask.
    Lpawns &= board.occEnemy & board.checkMask;
    Rpawns &= board.occEnemy & board.checkMask;

    // These pawns can walk Forward
    const U64 pawnsHV = pawns_mask & ~board.pinD;

    const U64 pawnsPinnedHV = pawnsHV & board.pinHV;
    const U64 pawnsUnPinnedHV = pawnsHV & ~board.pinHV;

    // Prune moves that are blocked by a piece
    const U64 singlePushUnpinned = shift<UP>(pawnsUnPinnedHV) & ~board.occAll;
    const U64 singlePushPinned = shift<UP>(pawnsPinnedHV) & board.pinHV & ~board.occAll;

    // Prune moves that are not on the checkmask.
    U64 singlePush = (singlePushUnpinned | singlePushPinned) & board.checkMask;

    U64 doublePush = ((shift<UP>(singlePushUnpinned & doublePushRank) & ~board.occAll) |
                      (shift<UP>(singlePushPinned & doublePushRank) & ~board.occAll)) &
                     board.checkMask;

    /********************
     * Add promotion moves.
     * These are always generated unless we only want quiet moves.
     *******************/
    if ((mt != Movetype::QUIET) && pawns_mask & RANK_BEFORE_PROMO)
    {
        U64 Promote_Left = Lpawns & RANK_PROMO;
        U64 Promote_Right = Rpawns & RANK_PROMO;
        U64 Promote_Move = singlePush & RANK_PROMO;

        while (Promote_Move)
        {
            Square to = poplsb(Promote_Move);
            movelist.Add(make<QUEEN, true>(to + DOWN, to));
            movelist.Add(make<ROOK, true>(to + DOWN, to));
            movelist.Add(make<KNIGHT, true>(to + DOWN, to));
            movelist.Add(make<BISHOP, true>(to + DOWN, to));
        }

        while (Promote_Right)
        {
            Square to = poplsb(Promote_Right);
            movelist.Add(make<QUEEN, true>(to + DOWN_LEFT, to));
            movelist.Add(make<ROOK, true>(to + DOWN_LEFT, to));
            movelist.Add(make<KNIGHT, true>(to + DOWN_LEFT, to));
            movelist.Add(make<BISHOP, true>(to + DOWN_LEFT, to));
        }

        while (Promote_Left)
        {
            Square to = poplsb(Promote_Left);
            movelist.Add(make<QUEEN, true>(to + DOWN_RIGHT, to));
            movelist.Add(make<ROOK, true>(to + DOWN_RIGHT, to));
            movelist.Add(make<KNIGHT, true>(to + DOWN_RIGHT, to));
            movelist.Add(make<BISHOP, true>(to + DOWN_RIGHT, to));
        }
    }

    // Remove the promotion pawns
    singlePush &= ~RANK_PROMO;
    Lpawns &= ~RANK_PROMO;
    Rpawns &= ~RANK_PROMO;

    /********************
     * Add single pushs.
     *******************/
    while (mt != Movetype::CAPTURE && singlePush)
    {
        Square to = poplsb(singlePush);
        movelist.Add(make<PAWN, false>(to + DOWN, to));
    }

    /********************
     * Add double pushs.
     *******************/
    while (mt != Movetype::CAPTURE && doublePush)
    {
        Square to = poplsb(doublePush);
        movelist.Add(make<PAWN, false>(to + DOWN + DOWN, to));
    }

    /********************
     * Add right pawn captures.
     *******************/
    while (mt != Movetype::QUIET && Rpawns)
    {
        Square to = poplsb(Rpawns);
        movelist.Add(make<PAWN, false>(to + DOWN_LEFT, to));
    }

    /********************
     * Add left pawn captures.
     *******************/
    while (mt != Movetype::QUIET && Lpawns)
    {
        Square to = poplsb(Lpawns);
        movelist.Add(make<PAWN, false>(to + DOWN_RIGHT, to));
    }

    /********************
     * Add en passant captures.
     *******************/
    if (mt != Movetype::QUIET && board.enPassantSquare != NO_SQ)
    {
        const Square ep = board.enPassantSquare;
        const Square epPawn = ep + DOWN;

        U64 epMask = (1ull << epPawn) | (1ull << ep);

        /********************
         * In case the en passant square and the enemy pawn
         * that just moved are not on the checkmask
         * en passant is not available.
         *******************/
        if ((board.checkMask & epMask) == 0)
            return;

        const Square kSQ = board.KingSQ(c);
        const U64 kingMask = (1ull << kSQ) & MASK_RANK[square_rank(epPawn)];
        const U64 enemyQueenRook = board.pieces(ROOK, ~c) | board.pieces(QUEEN, ~c);

        const bool isPossiblePin = kingMask && enemyQueenRook;
        U64 epBB = PawnAttacks(ep, ~c) & pawnsLR;

        /********************
         * For one en passant square two pawns could potentially take there.
         *******************/
        while (epBB)
        {
            Square from = poplsb(epBB);
            Square to = ep;

            /********************
             * If the pawn is pinned but the en passant square is not on the
             * pin mask then the move is illegal.
             *******************/
            if ((1ULL << from) & board.pinD && !(board.pinD & (1ull << ep)))
                continue;

            const U64 connectingPawns = (1ull << epPawn) | (1ull << from);

            /********************
             * 7k/4p3/8/2KP3r/8/8/8/8 b - - 0 1
             * If e7e5 there will be a potential ep square for us on e6.
             * However we cannot take en passant because that would put our king
             * in check. For this scenario we check if theres an enemy rook/queen
             * that would give check if the two pawns were removed.
             * If thats the case then the move is illegal and we can break immediately.
             *******************/
            if (isPossiblePin && (RookAttacks(kSQ, board.occAll & ~connectingPawns) & enemyQueenRook) != 0)
                break;

            movelist.Add(make<PAWN, false>(from, to));
        }
    }
}

inline U64 LegalKnightMoves(Square sq, U64 movableSquare)
{
    return KnightAttacks(sq) & movableSquare;
}

inline U64 LegalBishopMoves(const Board &board, Square sq, U64 movableSquare)
{
    // The Bishop is pinned diagonally thus can only move diagonally.
    if (board.pinD & (1ULL << sq))
        return BishopAttacks(sq, board.occAll) & movableSquare & board.pinD;
    return BishopAttacks(sq, board.occAll) & movableSquare;
}

inline U64 LegalRookMoves(const Board &board, Square sq, U64 movableSquare)
{
    // The Rook is pinned horizontally thus can only move horizontally.
    if (board.pinHV & (1ULL << sq))
        return RookAttacks(sq, board.occAll) & movableSquare & board.pinHV;
    return RookAttacks(sq, board.occAll) & movableSquare;
}

inline U64 LegalQueenMoves(const Board &board, Square sq, U64 movableSquare)
{
    U64 moves = 0ULL;

    if (board.pinD & (1ULL << sq))
        moves |= BishopAttacks(sq, board.occAll) & movableSquare & board.pinD;
    else if (board.pinHV & (1ULL << sq))
        moves |= RookAttacks(sq, board.occAll) & movableSquare & board.pinHV;
    else
    {
        moves |= RookAttacks(sq, board.occAll) & movableSquare;
        moves |= BishopAttacks(sq, board.occAll) & movableSquare;
    }

    return moves;
}

template <Movetype mt> U64 LegalKingMoves(const Board &board, Square sq)
{
    U64 bb;

    if (mt == Movetype::ALL)
        bb = board.enemyEmptyBB;
    else if (mt == Movetype::CAPTURE)
        bb = board.occEnemy;
    else
        bb = ~board.occAll;

    return KingAttacks(sq) & bb & ~board.seen;
}

template <Color c, Movetype mt> U64 LegalKingMovesCastling(const Board &board, Square sq)
{
    U64 bb;

    if (mt == Movetype::ALL)
        bb = board.enemyEmptyBB;
    else
        bb = ~board.occAll;

    U64 moves = KingAttacks(sq) & bb & ~board.seen;
    U64 emptyAndNotAttacked = ~board.seen & ~board.occAll;

    // clang-format off
    switch (c)
    {
    case White:
        if (    board.castlingRights & wk 
            &&  emptyAndNotAttacked & (1ULL << SQ_F1) 
            &&  emptyAndNotAttacked & (1ull << SQ_G1))
            moves |= (1ULL << SQ_H1);
        if (    board.castlingRights & wq 
            &&  emptyAndNotAttacked & (1ULL << SQ_D1) 
            &&  emptyAndNotAttacked & (1ull << SQ_C1) 
            &&  (1ull << SQ_B1) & ~board.occAll)
            moves |= (1ULL << SQ_A1);
        break;
    case Black:
        if (    board.castlingRights & bk 
            &&  emptyAndNotAttacked & (1ULL << SQ_F8) 
            &&  emptyAndNotAttacked & (1ull << SQ_G8))
            moves |= (1ULL << SQ_H8);
        if (    board.castlingRights & bq 
            &&  emptyAndNotAttacked & (1ULL << SQ_D8) 
            &&  emptyAndNotAttacked & (1ull << SQ_C8) 
            &&  (1ull << SQ_B8) & ~board.occAll)
            moves |= (1ULL << SQ_A8);
        break;
    default:
        return moves;
    }
    // clang-format on

    return moves;
}

// all legal moves for a position
template <Color c, Movetype mt> void legalmoves(Board &board, Movelist &movelist)
{
    init<c>(board, board.KingSQ(c));

    assert(board.doubleCheck <= 2);

    /********************
     * Moves have to be on the checkmask
     *******************/
    U64 movableSquare = board.checkMask;

    /********************
     * Slider, Knights and King moves can only go to enemy or empty squares.
     *******************/
    if (mt == Movetype::ALL)
        movableSquare &= board.enemyEmptyBB;
    else if (mt == Movetype::CAPTURE)
        movableSquare &= board.occEnemy;
    else // QUIET moves
        movableSquare &= ~board.occAll;

    Square from = board.KingSQ(c);
    U64 moves;

    if (mt == Movetype::CAPTURE || !board.castlingRights || board.checkMask != DEFAULT_CHECKMASK)
        moves = LegalKingMoves<mt>(board, from);
    else
        moves = LegalKingMovesCastling<c, mt>(board, from);

    while (moves)
    {
        Square to = poplsb(moves);
        movelist.Add(make<KING, false>(from, to));
    }

    /********************
     * Early return for double check as described earlier
     *******************/
    if (board.doubleCheck == 2)
        return;

    /********************
     * Prune knights that are pinned since these cannot move.
     *******************/
    U64 knights_mask = board.pieces(KNIGHT, c) & ~(board.pinD | board.pinHV);

    /********************
     * Prune horizontally pinned bishops
     *******************/
    U64 bishops_mask = board.pieces(BISHOP, c) & ~board.pinHV;

    /********************
     * Prune diagonally pinned rooks
     *******************/
    U64 rooks_mask = board.pieces(ROOK, c) & ~board.pinD;

    /********************
     * Prune double pinned queens
     *******************/
    U64 queens_mask = board.pieces(QUEEN, c) & ~(board.pinD & board.pinHV);

    /********************
     * Add the moves to the movelist.
     *******************/
    LegalPawnMovesAll<c, mt>(board, movelist);

    while (knights_mask)
    {
        Square from = poplsb(knights_mask);
        U64 moves = LegalKnightMoves(from, movableSquare);
        while (moves)
        {
            Square to = poplsb(moves);
            movelist.Add(make<KNIGHT, false>(from, to));
        }
    }

    while (bishops_mask)
    {
        Square from = poplsb(bishops_mask);
        U64 moves = LegalBishopMoves(board, from, movableSquare);
        while (moves)
        {
            Square to = poplsb(moves);
            movelist.Add(make<BISHOP, false>(from, to));
        }
    }

    while (rooks_mask)
    {
        Square from = poplsb(rooks_mask);
        U64 moves = LegalRookMoves(board, from, movableSquare);
        while (moves)
        {
            Square to = poplsb(moves);
            movelist.Add(make<ROOK, false>(from, to));
        }
    }

    while (queens_mask)
    {
        Square from = poplsb(queens_mask);
        U64 moves = LegalQueenMoves(board, from, movableSquare);
        while (moves)
        {
            Square to = poplsb(moves);
            movelist.Add(make<QUEEN, false>(from, to));
        }
    }
}

/********************
 * Entry function for the
 * Color template.
 *******************/
template <Movetype mt> void legalmoves(Board &board, Movelist &movelist, int start_index = 0)
{
    movelist.size = start_index;
    if (board.sideToMove == White)
        legalmoves<White, mt>(board, movelist);
    else
        legalmoves<Black, mt>(board, movelist);
}
} // namespace Movegen
