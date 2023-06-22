# Board

```cpp
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
        bool Board::inCheck();

        void Board::placePiece(Piece piece, Square sq);
        void Board::removePiece(Piece piece, Square sq);
        Piece Board::removePiece(Square sq);

        void Board::makeMove(const Move& move);
        void Board::unmakeMove(const Move& move);

        void Board::makeNullMove();
        void Board::unmakeNullMove();
};
```
