# PGN parsing

## Using Custom Visitors with pgn::StreamParser

This explains how to use the pgn::Visitor class to process PGN (Portable Game Notation) files using the pgn::StreamParser. By creating a custom visitor, you can define how each part of the PGN data is handled.

::: info
The visitor can parse multiple games in a single PGN file.
:::

### Creating a Custom Visitor

To process PGN data, inherit from the pgn::Visitor class and override the virtual methods that correspond to different parts of the PGN file.

::: tip
If you want to convert the `string_view move` to an internal move object, you can use `pgn::parseSan`.
See [here](/pages/move.md#other-formats) for more information.
:::

```cpp
class MyVisitor : public pgn::Visitor {
public:
    virtual ~MyVisitor() {}

    void startPgn() {
        // Called at the start of each PGN game
    }

    void header(std::string_view key, std::string_view value) {
        // Called for each header tag (e.g., [Event "F/S Return Match"])
    }

    void startMoves() {
        // Called before the moves of a game are processed
    }

    void move(std::string_view move, std::string_view comment) {
        // Called for each move in the game
    }

    void startVariation() {
        // Called at the opening parenthesis of a side variation
    }

    void endVariation() {
        // Called at the closing parenthesis of a side variation
    }

    void endPgn() {
        // Called at the end of each PGN game
    }
};
```

Parenthesized PGN variations are skipped by default, preserving the parser's previous
mainline-only behavior. Enable them with `parser.setParseVariations(true)`. When enabled,
the parser calls `startVariation()` and `endVariation()` around variation moves, including
nested variations. The visitor's `move()` method receives mainline and variation moves in
PGN order.

### Parsing PGN Files

To parse a PGN file using your custom visitor:

#### Open the PGN file

```cpp
std::ifstream file_stream("path/to/your/file.pgn");
if (!file_stream.is_open()) {
    // Handle error
    return -1;
}
```

Create an instance of your visitor:

```cpp
MyVisitor visitor;
```

Create the parser and parse the games:

```cpp
pgn::StreamParser parser(file_stream);
auto error = parser.readGames(visitor);
```

#### Putting it together

```cpp
# include <fstream>

# include "chess.hpp"

class MyVisitor : public pgn::Visitor {
public:
    virtual ~MyVisitor() {}

    void startPgn() {
        // Initialization code
    }

    void header(std::string_view key, std::string_view value) {
        // Process header tags
    }

    void startMoves() {
        // Prepare to process moves
    }

    void move(std::string_view move, std::string_view comment) {
        // Process each move
    }

    void endPgn() {
        // Cleanup code
    }
};

int main() {
    std::ifstream file_stream("path/to/your/file.pgn");
    if (!file_stream.is_open()) {
        // Handle error
        return -1;
    }

    MyVisitor visitor;
    pgn::StreamParser parser(file_stream);
    auto error = parser.readGames(visitor);

    if (error) {
        std::cerr << "Error parsing PGN: " << error.message() << std::endl;
        return -1;
    }

    return 0;
}
```
