# PGN Analysis

## PGN Parsing

Since 0.4.0 the pgn reader is now using a stream parser, which calls methods of a user implemented `Visitor` class. This allows for a more flexible and efficient parsing of the pgn file.

These methods will always receive a `std::string`, this means that if you want to parse a move to
an internal one, you need to call `pgn::parseSan` yourself.

The `Visitor` class is defined as follows:

```c++
class MyVisitor : public pgn::Visitor {
   public:
    void header(std::string_view key, std::string_view value) override {}

    void move(std::string_view move, std::string_view comment) override {}

    void startPgn() override {}
    void startMoves() override {}
    void endPgn() override {}
};
```
