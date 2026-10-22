# PGN parsing

Use `chess::pgn` to read games from a `std::istream`, including files and string streams.
Choose a visitor to process callbacks as input arrives, or `pgn::games` to iterate over
one owning game at a time. Neither interface updates a chess board automatically.

## Supported PGN features and limitations

The reader is a practical streaming parser, **not a fully PGN-compliant parser** and does not provide lossless PGN reading. A successful parse does not prove that the input is valid PGN
or that its moves are legal.

| Feature | Behavior |
| --- | --- |
| Tag pairs | Delivered as key/value strings. Required tags and their values are not validated. |
| Movetext | Delivered as move strings; move numbers are discarded. |
| Brace comments, `{text}` | Delivered without braces with the associated move. A comment before the first move can produce a callback with an empty move. |
| Recursive Annotation Variations (RAV), `( ... )` | Skipped by default. Enable `setParseVariations(true)` to receive variation boundaries and moves, including nested variations. |
| Numeric Annotation Glyphs (NAG), `$1`, `$2`, etc. | Separated NAG tokens after moves are skipped. There is no NAG callback or event, and their values are not retained or validated. |
| Symbolic annotations, `!`, `?`, `!?`, etc. | No dedicated annotation API; suffixes attached to moves remain in the move string. |
| Game termination | Recognizes the usual `1-0`, `0-1`, `1/2-1/2`, and `*` endings. No result callback or event is emitted; retain the `Result` tag if needed. |
| Multiple games | Supported in one input stream. |

Do not assume arbitrary PGN token layouts are accepted. In particular, write NAGs
as separate tokens (`e4 $1`, rather than `e4$1`); attached NAGs remain in the move
string. Semicolon line comments and percent escape lines are not handled as PGN
comments. Use brace comments instead. The reader expects games with tag pairs;
it is not a general reader for headerless movetext. Header keys, header values,
and move tokens have a 255-character buffer limit. Comments have no fixed token-length limit.
Malformed variations and incomplete input are not comprehensively diagnosed.

## Process games with a visitor

Implement the five required callbacks. Variation callbacks are optional and default
to doing nothing. This complete example prints the mainline of every game:

```cpp
#include <fstream>
#include <iostream>
#include <string_view>
#include "chess.hpp"

class PrintMoves : public chess::pgn::Visitor {
public:
    void startPgn() override {}
    void header(std::string_view key, std::string_view value) override {
        if (key == "Event") std::cout << value << '\n';
    }
    void startMoves() override {}
    void move(std::string_view san, std::string_view comment) override {
        if (!san.empty()) std::cout << san << ' ';
        if (!comment.empty()) std::cout << '{' << comment << "} ";
    }
    void endPgn() override { std::cout << '\n'; }
};

int main() {
    std::ifstream input("games.pgn");
    if (!input) {
        std::cerr << "Cannot open games.pgn\n";
        return 1;
    }

    PrintMoves visitor;
    chess::pgn::StreamParser parser(input);
    const auto error = parser.readGames(visitor);
    if (error) {
        std::cerr << "PGN error code: " << static_cast<int>(error.code()) << '\n';
        return 1;
    }
}
```

The usual callback sequence is `startPgn()`, `header(...)` for each tag,
`startMoves()`, `move(...)` for each move, then `endPgn()`. Empty games may have
no move callbacks. Callback string arguments, including comments, are non-owning `std::string_view`
values. Copy them into `std::string` if you need them after the callback returns.
The input stream must outlive the parser.

To filter games, call `skipPgn(true)` from a callback after the game starts, for
example in `header()` when a tag does not match your filter. Further header,
move, and variation callbacks are suppressed for that game; `endPgn()` still runs.
The skip flag is reset for the next game. Skipping still consumes the input.

### Variations and board state

Enable variations before reading:

```cpp
parser.setParseVariations(true);
```

Override `startVariation()` and `endVariation()` to track nesting. For example,
`1. e4 (1. d4 d5) e5 *` produces move `e4`, variation start, moves `d4` and `d5`,
variation end, and move `e5`. NAGs do not produce events in either mode.

If you replay these moves on a `Board`, a variation starts from the position
**before the preceding move**. Save the continuation position, undo that move for
the variation, then restore the saved position at the variation's end. Use a stack
for nested variations. The parser only reports boundaries; it does not manage this
board history for you.

### Convert SAN strings into moves

Use `chess::uci::parseSan(board, san)` with the position before the move, then
`board.makeMove(move)` to advance. Ignore callbacks with an empty SAN string.
Reset the board in `startPgn()` and apply a `FEN` header before processing moves
when the game starts from a custom position. Configure Chess960 on the board when
appropriate for your input. SAN parsing and board setup have their own error
handling, separate from PGN stream errors.

See [move formats](/pages/move#other-formats) and the [Board API](/pages/board).

## Iterate over owning games

`pgn::games` provides a lazy, single-pass range. Each `pgn::Game` contains owning
header strings and a flat sequence of events in PGN order. Enable variations before
iteration to retain their boundaries:

```cpp
std::ifstream input("games.pgn");
if (!input) return 1;
auto range = chess::pgn::games(input);
range.setParseVariations(true);

for (const chess::pgn::Game& game : range) {
    for (const auto& [key, value] : game.headers) {
        // Process each tag pair.
    }
    for (const auto& event : game.events) {
        if (event.type == chess::pgn::GameEventType::Move) {
            // event.san and event.comment are owning strings.
        } else if (event.type == chess::pgn::GameEventType::VariationStart) {
            // Enter a variation.
        } else {
            // Leave a variation.
        }
    }
}

if (range.error()) return 1;
```

The input stream must outlive the range. Incrementing the iterator replaces the
current game, invalidating references to its contents. Copy a `Game` to keep it.
The range cannot be restarted or used for independent simultaneous traversals.
An empty move string can represent a leading comment here too.

## Errors and incremental reading

`readGames(visitor)` consumes the stream and returns `StreamParserError`.
Alternatively, call `readNextGame(visitor)` repeatedly on one parser; it returns
`true` for a completed game and `false` at end of input or on error. Check
`parser.error()` after that loop to distinguish an error from normal exhaustion.
Use one reading interface per parser; do not interleave `readGames` with incremental
reads or read directly from the stream while the parser has buffered data.

Errors expose `code()`, `hasError()`, and a boolean conversion. The codes are
`None`, `ExceededMaxStringLength`, `InvalidHeaderMissingClosingBracket`,
`InvalidHeaderMissingClosingQuote`, and `NotEnoughData`. `readGames` reports
`NotEnoughData` for an empty stream; incremental reading simply reaches its end.
Currently `message()` does not implement `ExceededMaxStringLength` and can assert
for that code in debug builds, so the example reports the numeric code.

The parser can finish a game at end of input even without a termination marker.
Errors do not roll back callbacks already delivered. Check errors before treating
parsed data as a complete import, and validate SAN separately if legal moves matter.
