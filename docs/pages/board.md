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
Also checkout [GameResultReason](/pages/types), [GameResult](/pages/types) and [CheckType](/pages/types) enums.
:::

::: warning
If you need to undo a move you must pass the same move object that was used to make the move.
:::

## API

```cpp
using PackedBoard = std::array<std::uint8_t, 24>;

class Board {
    public:
        explicit Board(std::string_view fen = constants::STARTPOS, bool chess960 = false);

        static Board fromFen(std::string_view fen);
        static Board fromXfen(std::string_view xfen);
        static Board fromEpd(std::string_view epd);

        /**
         * @brief Returns true if the given FEN was successfully parsed and set
         * (position might still be illegal).
         * @param fen
         * @return
         */
        bool setFen(std::string_view fen);

        /**
         * @brief Returns true if the given xFEN was successfully parsed and set
         * (position might still be illegal).
         * @param xfen
         * @return
         */
        bool setXfen(std::string_view xfen);

        /**
         * @brief Returns true if the given EPD was successfully parsed and set
         * (position might still be illegal).
         * @param epd
         * @return
         */
        bool setEpd(const std::string_view epd);

        std::string getFen(bool moveCounters = true) const;
        std::string getXfen(bool moveCounters = true) const;
        std::string getEpd() const;

        /// @brief Make a move on the board. The move must be legal otherwise the
        /// behavior is undefined. EXACT can be set to true to only record
        /// the enpassant square if the enemy can legally capture the pawn on their
        /// next move.
        /// @param move
        /// @tparam EXACT
        /// @return
        template <bool EXACT = false>
        void makeMove(const Move move);
        void unmakeMove(const Move move);

        void makeNullMove();
        void unmakeNullMove();

        Bitboard us(Color color) const;
        Bitboard them(Color color) const;

        /// @brief recalculate all bitboards
        /// @return
        Bitboard all() const;

        /// @brief more efficient version of all(), which is incremental
        /// @return
        Bitboard occ() const;

        Square kingSq(Color color) const;

        Bitboard pieces(PieceType type, Color color) const;

        Bitboard pieces(PieceType type) const;

        template <typename... Pieces>
        Bitboard pieces(Pieces... pieces) const;

        /// @brief Checks if a move is a capture, enpassant moves are also considered captures.
        /// @param move
        /// @return
        bool isCapture(const Move move) const;

        /// @brief Returns either the piece or the piece type on a square
        /// @tparam T
        /// @param sq
        /// @return
        template <typename T = Piece>
        T at(Square sq) const;

        U64 hash() const;
        Color sideToMove() const;
        Square enpassantSq() const;
        CastlingRights castlingRights() const;
        std::uint32_t halfMoveClock() const;
        std::uint32_t fullMoveNumber() const;

        void set960(bool is960);
        bool chess960() const;

        std::string getCastleString() const;

        /**
         * @brief Checks if the current position is a repetition, set this to 1 if
         * you are writing a chess engine.
         * @param count
         * @return
         */
        bool isRepetition(int count = 2) const;

        /**
         * @brief Checks if the current position is a draw by 50 move rule.
         * Keep in mind that by the rules of chess, if the position has 50 half
         * moves it's not necessarily a draw, since checkmate has higher priority,
         * call getHalfMoveDrawType,
         * to determine whether the position is a draw or checkmate.
         * @return
         */
        bool isHalfMoveDraw() const;

        /**
         * @brief Only call this function if isHalfMoveDraw() returns true.
         * @return
         */
        std::pair<GameResultReason, GameResult> getHalfMoveDrawType() const;

        /**
         * @brief Basic check if the current position is a draw by insufficient material.
         * @return
         */
        bool isInsufficientMaterial() const;

        /**
         * @brief Checks if the game is over. Returns GameResultReason::NONE if the game is not over.
         * This function calculates all legal moves for the current position to check if the game is over.
         * If you are writing a chess engine you should not use this function.
         * @return
         */
        std::pair<GameResultReason, GameResult> isGameOver() const;

        /// @brief Checks if the square is attacked by the color.
        bool isAttacked(Square square, Color color) const;

        /// @brief Check if the current position is in check.
        bool inCheck() const;

        CheckType givesCheck(const Move &m) const;

        /// @brief Check if the color has any non pawn material left.
        bool hasNonPawnMaterial(Color color) const;

        /// @brief Recalculates the zobrist hash and return it.
        /// If you want get the zobrist hash use hash().
        U64 zobrist() const;

        Bitboard getCastlingPath(Color c, bool isKingSide) const;

        class Compact {
            public:
                /// @brief Compresses the board into a PackedBoard
                static PackedBoard encode(const Board &board);

                /// @brief Compresses a FEN into a PackedBoard
                /// @param fen
                /// @param chess960 If the board is a chess960 position, set this to true
                static PackedBoard encode(std::string_view fen, bool chess960 = false);

                /// @brief Creates a Board object from a PackedBoard
                /// @param compressed
                /// @param chess960 If the board is a chess960 position, set this to true
                static Board decode(const PackedBoard &compressed, bool chess960 = false);
        }
};
```
