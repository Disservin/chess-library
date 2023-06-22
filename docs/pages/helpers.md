# Helper Utilities

Below you will find a set of a few functions that can ease your life.

```cpp
namespace utils {
    void printBitboard(Bitboard bb);

    std::vector<std::string> splitString(
        const std::string& string,
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
