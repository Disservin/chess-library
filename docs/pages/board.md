# Board

## Description

The Board class represents the state of a chess game. It provides functionalities to manipulate the board, execute moves, and query the game state. This class is essential for handling game logic, including move generation,  and checking game-ending conditions like checkmate or stalemate.

Moves can be made using the `makeMove` function, which takes a `Move` object as an argument and also taken back using the `unmakeMove` function. The `makeNullMove` and `unmakeNullMove` functions are used to make and unmake null moves, respectively.

## PackedBoard

Commonly a chess board is represented by a FEN string, but it can also be represented by a PackedBoard. A PackedBoard only uses 24 bytes to represent the board, which is more memory-efficient than a FEN string. The `Compact` class provides functions to convert a Board object to a PackedBoard and vice versa.

::: info
The PackedBoard is not human-readable, and it is recommended to use the FEN string for debugging purposes.
:::

::: tip
Also checkout [GameResultReason](/pages/types) and [GameResult](/pages/types) enums.
:::

::: warning
If you need to undo a move you must pass the same move object that was used to make the move.
:::

## API

```cpp
using PackedBoard = std::array<std::uint8_t, 24>;

class Board {
    public:
        Board::Board(std::string_view fen)

        void setFen(std::string_view fen);
        std::string getFen(bool moveCounters = true);

        /// @brief Make a move on the board. The move must be legal otherwise the
        /// behavior is undefined. EXACT can be set to true to only record
        /// the enpassant square if the enemy can legally capture the pawn on their
        /// next move.
        /// @param move
        /// @tparam EXACT
        /// @return
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

        /**
         * @brief Checks if the current position is a repetition, set this to 1 if
         * you are writing a chess engine.
         * @param count
         * @return
         */
        bool isRepetition(int count = 2);

        /**
         * @brief Checks if the current position is a draw by 50 move rule.
         * Keep in mind that by the rules of chess, if the position has 50 half
         * moves it's not necessarily a draw, since checkmate has higher priority,
         * call getHalfMoveDrawType,
         * to determine whether the position is a draw or checkmate.
         * @return
         */
        bool isHalfMoveDraw();

        /**
         * @brief Only call this function if isHalfMoveDraw() returns true.
         * @return
         */
        std::pair<GameResultReason, GameResult> getHalfMoveDrawType();

        /**
         * @brief Basic check if the current position is a draw by insufficient material.
         * @return
         */
        bool isInsufficientMaterial();

        /**
         * @brief Checks if the game is over. Returns GameResultReason::NONE if the game is not over.
         * This function calculates all legal moves for the current position to check if the game is over.
         * If you are writing a chess engine you should not use this function.
         * @return
         */
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
