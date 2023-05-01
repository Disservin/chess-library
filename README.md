## Overview:

A chess library for C++.<br>
Reasonably fast move generation for all purposes.<br>
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

**There are also parallel implementations of perft, you can find these in the [parallel](https://github.com/Disservin/chess-library/tree/parallel) and [parallel-pgo](https://github.com/Disservin/chess-library/tree/parallel-pgo) branches.**

```
Parallel     : nodes 3195901860 time 585ms nps 5.453.757.440
Parallel-Pgo : nodes 3195901860 time 521ms nps 6.122.417.356
```

## Usage

This is a single/two header library.

You only need to include `chess.hpp` header and have the `sliders.hpp` header in the same directory.
Aftewards you can access the chess logic over the `Chess::` namespace.

**Currenlty chess960 is not fully implemented!**

## Types:

```c++
enum MoveGenType;
enum Square;
enum class Piece;
enum class PieceType;
enum class Rank;
enum class File;
enum CastlingRight;
enum class Direction;
enum class Color;
enum class GameResult;
```

## Constants

```c++
static const std::string STARTPOS = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
static constexpr int MAX_SQ = 64;
static constexpr int MAX_MOVES = 218;
```

## The Move Object

Castling moves are encoded as king captures rook. So always make sure the type is not castle
before checking for captures.

```c++
struct Move {
   public:
    Move() = default;

    explicit Move(uint16_t move) : move_(move){};

    template <uint16_t MoveType = 0>
    static Move make(Square source, Square target, PieceType pt = PieceType::KNIGHT);

    constexpr Square from() const;

    constexpr Square to() const;
    constexpr uint16_t typeOf() const;

    constexpr PieceType promotionType() const;

    constexpr const uint16_t move() const ;
    constexpr uint16_t &move();

    constexpr bool operator==(const Move &right) const;
    constexpr bool operator!=(const Move &right) const;

    static constexpr uint16_t NO_MOVE = 0;
    static constexpr uint16_t NULL_MOVE = 65;

    static constexpr uint16_t NORMAL;
    static constexpr uint16_t PROMOTION;
    static constexpr uint16_t EN_PASSANT;
    static constexpr uint16_t CASTLING ;

    friend std::ostream &operator<<(std::ostream &os, const Move &m);
};
```

### Creation

```c++
Move m = Move::make<Move::NORMAL>(Square::SQ_E2, Square::SQ_E4);
```

## ExtMove Object

This is more or less the same as the Move object, but it has a score field.

```c++
struct ExtMove : public Move {
   public:
    ExtMove() = default;

    explicit ExtMove(uint16_t move) : Move(move){};

    template <uint16_t MoveType = 0>
    static ExtMove make(Square source, Square target, PieceType pt = PieceType::KNIGHT);

    constexpr int score() const;
    constexpr void setScore(int score);

    constexpr bool operator<(const ExtMove &right) const;
    constexpr bool operator>(const ExtMove &right) const;
};
```

## The Movelist Object

```c++

template <typename T>
struct Movelist {
   public:
    constexpr void add(T move);
    // returns the index of the move in the list, or -1 if not found
    constexpr int find(T m);

    constexpr void clear();
    constexpr int size() const;
};
```

## Intrinsic Functions

```c++
// returns the index of the least significant bit
constexpr int lsb(U64 bb);

// returns the index of the most significant bit
constexpr int msb(U64 bb);

// returns the number of set bits
constexpr int popcount(U64 bb);

// returns the lsb and pop it
constexpr int poplsb(U64 &bb);
```

## Attacks

```c++
namespace Attacks {

static constexpr U64 PAWN(Color c, Square sq);
static constexpr U64 KNIGHT(Square sq);
static constexpr U64 BISHOP(Square sq, U64 occ);
static constexpr U64 ROOK(Square sq, U64 occ);
static constexpr U64 QUEEN(Square sq, U64 occ);
static constexpr U64 KING(Square sq);

}
```

## Helper Functions

```c++

/// @brief Gets the file index of the square where 0 is the a-file
/// @param sq
/// @return the file of the square
constexpr File squareFile(Square sq);

/// @brief Gets the rank index of the square where 0 is the first rank.
/// @param sq
/// @return the rank of the square
constexpr Rank squareRank(Square sq);

/// @brief makes a square out of rank and file
/// @param f
/// @param r
/// @return
constexpr Square fileRankSquare(File f, Rank r);

/// @brief distance between two squares
/// @param a
/// @param b
/// @return
inline uint8_t squareDistance(Square a, Square b);

constexpr uint8_t diagonalOf(Square sq);

constexpr uint8_t antiDiagonalOf(Square sq);

/// @brief manhatten distance between two squares
/// @param sq1
/// @param sq2
/// @return
inline uint8_t manhattenDistance(Square sq1, Square sq2);

/// @brief color of a square, has nothing to do with whose piece is on that square
/// @param square
/// @return
constexpr Color getSquareColor(Square square);

constexpr Square relativeSquare(Color c, Square s);

constexpr Piece makePiece(Color c, PieceType pt);

constexpr PieceType typeOfPiece(Piece piece);

/// checks if the squares have the same color
bool sameColor(Square sq1, Square sq2);

/// prints a bitboard to the console
void printBitboard(U64 bb);
```

## The Position Object

```c++

class Board {
   public:
    explicit Board(const std::string &fen);

    void loadFen(const std::string &fen);
    std::string getFen() const;

    void makeMove(const Move &move);
    void unmakeMove(const Move &move);
    void makeNullMove();
    void unmakeNullMove();

    U64 us(Color c) const;

    U64 them(Color c) const;

    U64 all() const;

    Square kingSq(Color c) const;

    template <PieceType type, Color color>
    U64 pieces() const;

    template <Color color>
    U64 pieces(PieceType type) const;

    template <PieceType type>
    U64 pieces(Color color) const;

    constexpr U64 pieces(PieceType type, Color color) const;

    constexpr Piece pieceAt(Square square) const;

    static constexpr Color colorOfPiece(Piece piece);

    constexpr Color colorOfPiece(Square square) const;

    uint64_t hash() const;
    Color sideToMove() const;
    Square enpassantSquare() const;
    uint8_t castlingRights() const;

    /// TODO
    /// @return
    bool chess960() const;

    U64 occ() const;

    /// @brief every position has at least occured once, so a game will be drawn if its seen 2 times
    /// after the intial one
    /// @param count
    /// @return
    bool isRepetition(int count = 2) const;

    /// @brief first = reason why the game is over, "" if not over
    /// @param os
    /// @param b
    /// @return
    std::pair<std::string, GameResult> isGameOver() const;

    /// Is the square attacked by color c?
    /// @param square
    /// @param c
    /// @return
    bool isSquareAttacked(Square square, Color c) const;

    /// Is the current sidetomoves king attacked?
    /// @return
    bool isKingAttacked() const;

    std::string uci(const Move &move) const;
    Move uciToMove(const std::string &uci) const;

    // convert a move to san/lan notation
    std::string san(const Move &move);
    std::string lan(const Move &move);
};
```

## Move Generation

```c++
namespace MoveGen {
    template <typename T, MoveGenType mt>
    void legalmoves(Movelist<T> &movelist, const Board &board);

    template <typename T>
    bool isLegal(const Board &board, const T &move);
}
```

### Creation

```c++
Movelist<Move> movelist;
MoveGen::legalmoves<Move, MoveGenType::ALL>(movelist, board);

for (const auto &move : movelist) {
    // do something with move
}
```
