# PGN utilities

The `chess::pgn` namespace offers two interfaces to the same streaming reader.
See the [PGN parsing guide](/pages/pgn-parsing) for complete examples, supported
features, and format limitations, including RAV and NAG handling.

## StreamParser

| Member | Purpose |
| --- | --- |
| `StreamParser(std::istream&)` | Read from an existing stream, which must outlive the parser. |
| `setParseVariations(bool)` | Enable nested variation callbacks; disabled by default. |
| `readGames(Visitor&)` | Consume games and return a `StreamParserError`. |
| `readNextGame(Visitor&)` | Read one game; return false on exhaustion or error. |
| `error() const` | Inspect the error from incremental reading. |

## Visitor

Derive from `Visitor` and implement the required callbacks:

```cpp
void startPgn() override;
void header(std::string_view key, std::string_view value) override;
void startMoves() override;
void move(std::string_view san, std::string_view comment) override;
void endPgn() override;
```

Optional callbacks `startVariation()` and `endVariation()` delimit side lines when
variation parsing is enabled. They have empty default implementations.

`skipPgn(true)` suppresses further data callbacks for the current game, but still
calls `endPgn()`. Call it after the game starts. `skip()` returns the current flag;
it resets when the game ends.

Callback strings are borrowed views. Copy them to retain them. A move can be empty
when a comment precedes the movetext. Convert nonempty SAN strings with
`chess::uci::parseSan(board, san)`; PGN parsing does not maintain a board or check
move legality.

## Game range and events

`games(std::istream&)` returns a `GameRange` with `begin()`, `end()`,
`setParseVariations(bool)`, and `error()`. Configure variations before iteration.

```cpp
enum class GameEventType { Move, VariationStart, VariationEnd };

struct GameEvent {
    GameEventType type;
    std::string san;
    std::string comment;
};

struct Game {
    std::vector<std::pair<std::string, std::string>> headers;
    std::vector<GameEvent> events;
};
```

Headers retain their input order. Events retain move order and variation nesting;
variation boundary events have empty SAN and comment strings. NAG values and
movetext result markers are not retained. The range owns only its current game;
copy that game before advancing if you need to keep it.

## StreamParserError

`code()` returns one of `None`, `ExceededMaxStringLength`,
`InvalidHeaderMissingClosingBracket`, `InvalidHeaderMissingClosingQuote`, or
`NotEnoughData`. `hasError()` and the boolean conversion report whether the code
is nonzero. See the guide's [error handling notes](/pages/pgn-parsing#errors-and-incremental-reading)
for limits on diagnostics and `message()`.
