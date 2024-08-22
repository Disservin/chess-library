# Board

```cpp
using PackedBoard = std::array<std::uint8_t, 24>;

class Board {
    public:
        Board::Board(std::string_view fen)

        void setFen(std::string_view fen);
        std::string getFen(bool moveCounters = true);

        void makeMove(const Move move);
        void unmakeMove(const Move move);

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

        /// @brief Checks if a move is a capture, enpassant moves are also considered captures.
        /// @param move
        /// @return
        bool isCapture(const Move move);

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

        /// @brief Checks if the current position is a draw by 50 move rule. Keep in mind
        /// that by the rules of chess, if the position has 50 half moves it's not
        /// necessarily a draw, since checkmate has higher priority, call
        /// getHalfMoveDrawType, to determine whether the position is a draw or
        /// checkmate.
        bool isHalfMoveDraw();

        /// @brief Only call this function if isHalfMoveDraw() returns true.
        std::pair<GameResultReason, GameResult> getHalfMoveDrawType();

        bool isInsufficientMaterial();

        /// @brief Checks if the game is over.
        /// Returns GameResultReason::NONE if the game is not over.
        /// This function calculates all legal moves for the current position
        /// to check if the game is over.
        /// If you are writing you should not use this function.
        std::pair<GameResultReason, GameResult> isGameOver();

        /// @brief Checks if the square is attackeqd by the color.
        bool isAttacked(Square square, Color color);

        /// @brief Check if the current position is in check.
        bool inCheck();

        /// @brief Check if the color has any non pawn material left.
        bool hasNonPawnMaterial(Color color);

        /// @brief Recalculates the zobrist hash and return it.
        /// If you want get the zobrist hash use hash().
        U64 zobrist();

        class Compact {
            public:
                /// @brief Compresses the board into a PackedBoard
                static PackedBoard encode(const Board &board);

                /// @brief Creates a Board object from a PackedBoard
                /// @param compressed
                /// @param chess960 If the board is a chess960 position, set this to true
                static Board decode(const PackedBoard &compressed, bool chess960 = false);
        }
};
```
