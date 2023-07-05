# Extending the library

## Board

Sometimes you have to modify the library a bit to allow new features to be added, which only apply
to your use case.

The first step thing that you should consider when extending a library, is to not modify the original sources, unless you
absolutely have to.  
This is so that a) you dont break stuff by accident and b) you can better update the dependency.

### Incremental Updates

Often when working with chess engines you have to keep track of information incrementally between moves to keep good
performance.

Internally the `makeMove` and `unmakeMove` functions make use of `placePiece` and `removePiece` to update pieces
on the board.

Normally your new logic would go into these functions but since you shouldnt modify these, you can simply create a wrapper class (here called `W_Board`) which inherits from `Board` and extends
the `placePiece` and `removePiece` functions with the desired logic.

You will still need to call the original function to not break the library!

Also keep in mind that you probably have to reset your data in `setFen` again, because this
internally will use of the `placePiece` function.

Finally you will end up with something like this:

```cpp
#include <iostream>

#include "chess.hpp"

using namespace chess;

class W_Board : public Board {
   public:
    W_Board() : Board() {}
    W_Board(const std::string &fen) : Board(fen) {}

    virtual void setFen(std::string fen) {
        Board::setFen(fen);
        inc = 0;
    }

    int inc = 0;

   protected:
    void placePiece(Piece piece, Square sq) {
        Board::placePiece(piece, sq);
        inc += 1;
    }

    void removePiece(Piece piece, Square sq) {
        Board::removePiece(piece, sq);
        inc -= 1;
    }
};

int main() {
    W_Board board = W_Board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    std::cout << "board: " << board << std::endl;

    return 0;
}
```

If this was still not enough for you, think about adding the desired functionality back to master, in case
they are universal enough.
