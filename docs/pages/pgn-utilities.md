# PGN Analysis

## Read a Game

With the following function you can read the next game from a file conviently.

```cpp

/// @brief Read the next game from a file
/// @param file
/// @return
std::optional<Game> readGame(std::istream &file);
```

## Example

```cpp
void foo(const std::string& file) {
    std::ifstream pgn_file(file);

    std::string line;

    while (true) {
        auto game = pgn::readGame(pgn_file);

        if (!game.has_value()) {
            break;
        }

        // do something with Game
    }

    pgn_file.close();
}
```

## PGN Move

```cpp
struct PgnMove {
    Move move;
    std::string comment;
};
```

## Game Object

```cpp
struct Game {
   public:
    Game() = default;

    Game(const std::unordered_map<std::string, std::string> &headers, const std::vector<PgnMove> &moves);

    const std::unordered_map<std::string, std::string> &headers() const;

    const std::vector<PgnMove> &moves() const;
    std::vector<PgnMove> &moves();
};
```

::: tip
Keep in mind that `Game` is not a `Board` and it only stores the plain data extracted from the pgn.
If you want to replay all the moves you still have to create a Board object and set the correct starting
fen, as well as maybe enabling chess960.
:::
