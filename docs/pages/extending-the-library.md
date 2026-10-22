# Extending the library

`Board` exposes protected virtual `placePiece` and `removePiece` hooks, plus
virtual `setFen`, for applications that maintain extra state. Override the hooks
and call the base implementation to preserve the board’s own updates.

## Incremental piece count

This example tracks the total number of pieces. Construction of the base `Board`
does not invoke derived hooks, so the constructor initializes the count from the
finished board. A later `setFen` resets the count **before** loading, because
loading calls `placePiece` for each piece without removing the old pieces through
`removePiece`.

```cpp
#include <iostream>
#include <string_view>
#include "chess.hpp"

class CountingBoard : public chess::Board {
public:
    explicit CountingBoard(std::string_view fen = chess::constants::STARTPOS)
        : chess::Board(fen), pieceCount_(occ().count()) {}

    bool setFen(std::string_view fen) override {
        pieceCount_ = 0;
        return chess::Board::setFen(fen);
    }

    int pieceCount() const { return pieceCount_; }

protected:
    void placePiece(chess::Piece piece, chess::Square square) override {
        chess::Board::placePiece(piece, square);
        ++pieceCount_;
    }

    void removePiece(chess::Piece piece, chess::Square square) override {
        chess::Board::removePiece(piece, square);
        --pieceCount_;
    }

private:
    int pieceCount_ = 0;
};

int main() {
    CountingBoard board;
    std::cout << board.pieceCount() << '\n'; // 32
}
```

These are piece-placement hooks, not notifications for every state change. Null
moves do not place or remove pieces. Operations that directly replace board state,
such as `backtrackTo`, do not replay the hooks. `setXfen` also needs explicit
handling of the counter reset; `setEpd` delegates to virtual `setFen`. Rebuild or
restore your extra state when using such operations, and account for constructor
and copy behavior in your own wrapper.

`Board` has no virtual destructor. Keep ownership in the concrete derived type;
do not delete a derived instance through a `Board*`.
