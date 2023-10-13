# PGN Utilities

## PGN Parsing

Since 0.4.0 the pgn reader is now using a stream parser, which calls methods of a user implemented `Visitor` class. This allows for a more flexible and efficient parsing of the pgn file.

These methods will always receive a `std::string`, this means that if you want to parse a move to
an internal one, you need to call `pgn::parseSan` yourself.

The `Visitor` class is defined as follows:

::: tip
If you want to convert the `string_view move` to an internal move object, you can use `pgn::parseSan`.
See [here](/pages/move-object.md#other-formats) for more information.
:::

```c++
/// @brief Visitor interface for parsing PGN files
/// the order of the calls is as follows:
class Visitor {
   public:
    virtual ~Visitor(){};

    /// @brief When true, the current PGN will be skipped and only
    /// endPgn will be called, this will also reset the skip flag to false.
    /// Has to be called after startPgn.
    /// @param skip
    void skipPgn(bool skip);
    bool skip();

    /// @brief Called when a new PGN starts
    virtual void startPgn() = 0;

    /// @brief Called for each header
    /// @param key
    /// @param value
    virtual void header(std::string_view key, std::string_view value) = 0;

    /// @brief Called before the first move of a game
    virtual void startMoves() = 0;

    /// @brief Called for each move of a game
    /// @param move
    /// @param comment
    virtual void move(std::string_view move, std::string_view comment) = 0;

    /// @brief Called when a game ends
    virtual void endPgn() = 0;
};
```

So you will need to create a derived class from it.

```c++
class MyVisitor : public pgn::Visitor {
   public:
    virtual ~MyVisitor() {}

    void startPgn() { board.setFen(STARTPOS); }

    void header(std::string_view key, std::string_view value) {}

    void startMoves() {}

    void move(std::string_view move, std::string_view comment) {}

    void endPgn() {}

   private:
    Board board;
};
```
