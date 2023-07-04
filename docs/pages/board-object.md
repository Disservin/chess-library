# Board

```cpp
class Board {
    public:
        Board::Board(const std::string fen)

        void setFen(std::string fen);
        std::string getFen();

        void makeMove(const Move &move);
        void unmakeMove(const Move &move);

        void makeNullMove();
        void unmakeNullMove();

        Bitboard us(Color color);
        Bitboard them(Color color);



        /// @brief recalculate all bitboards
        /// @return
        Bitboard all();

        /// @brief more efficient version of all(), which is incremental
        /// @return
        Bitboard occ();

        Square kingSq(Color color);

        Bitboard pieces(PieceType type, Color color);

        Bitboard pieces(PieceType type);

        /// @brief Returns either the piece or the piece type on a square
        /// @tparam T
        /// @param sq
        /// @return
        template <typename T = Piece>
        T at(Square sq);

        Color color(Piece piece);

        U64 hash();
        Color sideToMove();
        Square enpassantSq();
        CastlingRights castlingRights();
        int halfMoveClock();
        int fullMoveNumber();

        void set960(bool is960);
        bool chess960();

        std::string getCastleString();

        bool isRepetition(int count = 2);

        std::pair<std::string, GameResult> isGameOver();

        bool isAttacked(Square square, Color color);

        bool inCheck();

        U64 zobrist();
};
```
