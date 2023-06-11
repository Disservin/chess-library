## Overview:

A chess library for C++, with reasonably fast move generation for all purposes.  
Also supports FRC/DFRC. Make sure that these are correct and include the file of the rook  
not K/Q/k/q or other annotations. Only a,b,c,d,e,f,g,h and uppercase variants are valid.
Tested on Ryzen 9 5950X:

```
depth 7  time 8088  nodes 3195901860   nps 395092330 fen rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1
depth 5  time 376   nodes 193690690    nps 513768408 fen r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1
depth 7  time 583   nodes 178633661    nps 305879556 fen 8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1
depth 6  time 1396  nodes 706045033    nps 505400882 fen r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1
depth 5  time 185   nodes 89941194     nps 483554806 fen rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8
depth 5  time 330   nodes 164075551    nps 495696528 fen r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 1

averaged:
nodes 4528287989 nps 412976560
```

## Usage

This is a single/two header library.

You only need to include `chess.hpp` header!
Aftewards you can access the chess logic over the `chess::` namespace.

## Types:

```c++
enum MoveGenType;
enum Square;
enum class Piece;
enum class PieceType;
enum class Rank;
enum class File;
enum class CastleSide;
enum class Direction;
enum class Color;
enum class GameResult;
```

## Constants

```c++
constexpr int MAX_SQ = 64;
constexpr int MAX_PIECE = 12;
constexpr int MAX_MOVES = 256;
constexpr Bitboard DEFAULT_CHECKMASK = 18446744073709551615ULL;

static const std::string STARTPOS = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

```

## The Move Object

Castling moves are encoded as king captures rook. So always make sure the type is not castle
before checking for captures.

```c++
struct Move {
   public:
    Move() = default;
    Move(uint16_t move) : move_(move), score_(0) {}

    template <uint16_t MoveType = 0>
    static constexpr Move make(Square source, Square target,
                                             PieceType pt = PieceType::KNIGHT);

    Square from() const;

    Square to() const;

    uint16_t typeOf() const;

    PieceType promotionType() const;

    uint16_t move() const;
    int16_t score() const;

    bool operator==(const Move& rhs) const { return move_ == rhs.move_; }
    bool operator!=(const Move& rhs) const { return move_ != rhs.move_; }

    static constexpr uint16_t NO_MOVE = 0;
    static constexpr uint16_t NULL_MOVE = 65;
    static constexpr uint16_t NORMAL = 0;
    static constexpr uint16_t PROMOTION = 1 << 14;
    static constexpr uint16_t ENPASSANT = 2 << 14;
    static constexpr uint16_t CASTLING = 3 << 14;

    friend std::ostream& operator<<(std::ostream& os, const Move& move);

   private:
    uint16_t move_;
    int16_t score_;
};
```

### Creation

```c++
Move m = Move::make<Move::NORMAL>(Square::SQ_E2, Square::SQ_E4);
```

## The Movelist Object

```c++

struct Movelist {
   public:
    constexpr void add(Move move);

    // returns -1 if not found
    constexpr int find(Move move);

    constexpr int size() const;
    constexpr void clear();
    inline void sort(int index = 0);

    constexpr Move operator[](int index) const;
    constexpr Move& operator[](int index);

   private:
    Move moves_[MAX_MOVES];
    uint8_t size_ = 0;
};
```

## Intrinsic Functions

```c++
namespace builtin {
// returns the index of the least significant bit
constexpr int lsb(Bitboard bb);

// returns the index of the most significant bit
constexpr int msb(Bitboard bb);

// returns the number of set bits
constexpr int popcount(Bitboard bb);

// returns the lsb and pop it
constexpr int poplsb(Bitboard &bb);
}

```

## Attacks

```c++
namespace movegen {
    namespace attacks {
        static constexpr Bitboard pawn(Color c, Square sq);
        static constexpr Bitboard knight(Square sq);
        static constexpr Bitboard bishop(Square sq, Bitboard occ);
        static constexpr Bitboard rook(Square sq, Bitboard occ);
        static constexpr Bitboard queen(Square sq, Bitboard occ);
        static constexpr Bitboard king(Square sq);
    }
}
```

## Helper Functions

```c++
namespace utils {
    void printBitboard(Bitboard bb);

    std::vector<std::string> splitString(const std::string& string,
                                                          const char& delimiter);
    File squareFile(Square sq);
    Rank squareRank(Square sq);

    int squareDistance(Square a, Square b);

    int diagonalOf(Square sq);

    int antiDiagonalOf(Square sq);

    void trim(std::string& s);

    PieceType typeOfPiece(Piece piece);

    Square relativeSquare(Color c, Square s);

    Piece makePiece(Color c, PieceType pt);

    Square fileRankSquare(File f, Rank r);

    bool sameColor(Square sq1, Square sq2);
}
```

## The Position Object

```c++

class Board {
    public:
        Board::Board(const std::string fen)

        void Board::setFen(std::string fen)
        std::string Board::getFen();

        U64 Board::zobrist();

        std::string Board::getCastleString();

        bool Board::isRepetition(int count);

        std::pair<std::string, GameResult> Board::isGameOver();

        bool Board::isAttacked(Square square, Color color);
        bool Board::isKingAttacked();

        void Board::placePiece(Piece piece, Square sq);
        void Board::removePiece(Piece piece, Square sq);
        Piece Board::removePiece(Square sq);

        void Board::makeMove(const Move& move);
        void Board::unmakeMove(const Move& move);

        void Board::makeNullMove();
        void Board::unmakeNullMove();
};
```

## Move Generation

```c++
namespace MoveGen {
    template <MoveGenType mt>
    void legalmoves(Movelist& movelist, const Board& board);
}
```

### Movelist Usage

```c++
Movelist movelist;
movegen::legalmoves(movelist, board)

for (const auto &move : movelist) {
    // do something with move
}
```
