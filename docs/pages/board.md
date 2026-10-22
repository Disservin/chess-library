# Board

## Description

The Board class represents the state of a chess game. It provides functionalities to manipulate the board, execute moves, and query the game state. This class is essential for handling game logic, including move generation,  and checking game-ending conditions like checkmate or stalemate.

Moves can be made using the `makeMove` function, which takes a `Move` object as an argument and also taken back using the `unmakeMove` function. The `makeNullMove` and `unmakeNullMove` functions are used to make and unmake null moves, respectively.

## Loading positions and Chess960

A default `Board` uses the standard starting position. `setFen`, `setXfen`, and
`setEpd` return a success flag; parsing success does not prove the position is
legal or reachable. A failed setter can leave a partially changed board. Parse
into a temporary board if you need to preserve the original on failure.
Factory methods return a board without exposing a success flag.

`Board(fen, true)` interprets castling rights for Chess960. `setXfen` also enables
Chess960: `KQkq` selects the outermost rook on the corresponding side of the king,
while file letters select specific rook files. `getXfen` writes that convention.
Castling-type prefixes such as `s` and `m` are not supported.

`set960(bool)` reparses the last loaded FEN, resetting the position and history;
it is not a switch to toggle in the middle of play. Configure the mode when
loading a position. It does not choose or generate a Chess960 starting array.

## History, draws, and checks

Undo normal moves in reverse order using the same moves passed to `makeMove`.
This relies on the board’s saved history; loading a FEN starts a new history and
cannot reconstruct earlier moves or repetitions. Null moves are search operations,
not legal chess moves, and have their own paired make/unmake functions.

`isRepetition(2)` (the default) looks for two earlier occurrences of the current
position, giving three occurrences in total. `isRepetition(1)` checks for one
earlier occurrence, which can be useful in engine search. Only recorded history
is available. `isHalfMoveDraw()` tests for at least 100 half-moves (50 moves by
each side) without a pawn move or capture. Call `getHalfMoveDrawType()` only when
that test is true, since checkmate takes priority.

`isGameOver()` combines the library’s material, repetition, fifty-move, and legal
move checks. Its result is from the side-to-move perspective. It does not manage
draw claims, agreements, clocks, resignation, or a complete tournament ruleset.
`isInsufficientMaterial()` is a basic material test, not a general dead-position
solver. Engines that already generated legal moves can inspect that list and
`inCheck()` instead of repeating the game-over move query.

`checkers()` returns the enemy pieces checking the side-to-move king;
`inDoubleCheck()` tests whether there are two. `givesCheck(move)` tests a legal
move without playing it. See [CheckType](/pages/types#checktype) for detailed mode.

`hash()` reads the maintained position hash; `zobrist()` recomputes it.
`zobristAfter(move)` predicts a legal move’s hash without changing the board.
Its `EXACT = false` specialization skips en-passant and castling updates and is
not a complete resulting position key. `operator==` compares board state but not
saved history; `fullyEquals()` also compares history.

## Occupancy, captures, and castling rights

`us(color)` returns that color’s occupied squares and `them(color)` returns the
opposite color’s squares; neither implicitly selects the side to move.
`pieces(type)` includes both colors. Combine piece types with
`pieces(PieceType::BISHOP, PieceType::QUEEN)`, or select one color with
`pieces(type, color)`. `at(square)` returns a `Piece`; `at<PieceType>(square)`
returns only its type. Empty squares return the corresponding `NONE` value.

Query `isCapture(move)` and `getCapturing(move)` before making the move. They
handle en passant, where the captured pawn is not on the destination, and exclude
castling despite its internal king-to-rook encoding.

`castlingRights()` returns a copy of `Board::CastlingRights`. Use
`has(color, Side::KING_SIDE)` or `has(color, Side::QUEEN_SIDE)` to test a right,
`getRookFile(color, side)` to locate its rook, and `isEmpty()` to test whether no
rights remain. `Side` is nested in `Board::CastlingRights`. Having a right does
not mean castling is legal now; check the legal move list for that. Modifying the
returned copy does not modify the board.

## PackedBoard

Commonly a chess board is represented by a FEN string, but it can also be represented by a PackedBoard. A PackedBoard uses 24 bytes to represent piece placement, side to move, en passant, and castling rights, which is more memory-efficient than a FEN string. The `Compact` class provides functions to convert a Board object to a PackedBoard and vice versa.

::: info
The PackedBoard is not human-readable, and it is recommended to use the FEN string for debugging purposes.
:::

::: tip
Also checkout [GameResultReason](/pages/types), [GameResult](/pages/types) and [CheckType](/pages/types) enums.
:::

::: warning
If you need to undo a move you must pass the same move object that was used to make the move.
:::

Packed boards omit half-move and full-move counters and repetition history.
Decoding resets the counters to half-move 0 and full-move 1. Pass the correct
Chess960 flag when decoding; the format does not retain that mode for you.
Use a full `Board` copy when you need to preserve game history and counters.

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
        virtual bool setFen(std::string_view fen);

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

        /// @brief Check if a move is legal from the current position. Assumes there
        /// is some position where the move is legal (e.g., no promotion to a king).
        /// @param move
        /// @return
        bool isLegal(const Move move) const;

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
        void unmakeMove(const RevMove& reverse);

        void makeNullMove();
        void unmakeNullMove();


        /**
        * @brief Backtrack the board state to a previous state. Assumes that the current
        * state was reached from previous by a series, S, of makeMove() calls. Otherwise
        * the behavior is undefined. Equivalent to calling unmakeMove() for every move of
        * S. This method can be useful in tree-searching algorithms that
        * repeatedly backtrack from leaf nodes to the root node, like MCTS.
        * @param previous
        */
        void backtrackTo(const Board& previous);

        Bitboard us(Color color) const;
        Bitboard them(Color color) const;

        /// @brief Occupancy of both colors.
        /// @return
        Bitboard all() const;

        /// @brief Occupancy of both colors; equivalent to all().
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

        /// @brief Returns the captured piece or piece type of a move
        /// @tparam T
        /// @param move
        /// @return
        template <typename T = Piece>
        T getCapturing(const Move move) const;

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
         * Keep in mind that by the rules of chess, if the position has 100 half
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

        bool inDoubleCheck() const;
        Bitboard checkers() const;

        template <bool Detail = false>
        CheckType givesCheck(Move move) const;

        /// @brief Check if the color has any non pawn material left.
        bool hasNonPawnMaterial(Color color) const;

        /// @brief Recalculates the zobrist hash and return it.
        /// If you want get the zobrist hash use hash().
        U64 zobrist() const;

        template <bool EXACT = true>
        U64 zobristAfter(const Move& move) const;

        bool operator==(const Board& other) const;
        bool fullyEquals(const Board& other) const;

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
        };
};
```

## Reverse move generation

`RevMoveGen::genMoves` generates pseudo-legal forward moves that could have
produced the current board. A `RevMove` stores the forward `Move` and the
captured `Piece`, since the move encoding alone does not identify a capture.
Pass a generated entry to `Board::unmakeMove(const RevMove&)` to restore its
predecessor piece placement without a recorded move history.

```cpp
std::vector<chess::RevMove> reverseMoves;
chess::RevMoveGen::genMoves(reverseMoves, board);

for (const auto& reverse : reverseMoves) {
    auto predecessor = board;
    predecessor.unmakeMove(reverse);
    // Check predecessor legality or continue the proof game search.
}
```

The generator uses geometric move rules, so candidates still need predecessor
legality checks. A FEN does not retain every castling right, prior en-passant
square, or exact half-move clock that was lost by the forward move; reverse undo
infers castling rights where possible, restores the en-passant square for an
en-passant move, and uses a conservative clock.
