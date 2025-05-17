# An extensive SHL Chess Library for C++

[![Chess Library](https://github.com/Disservin/chess-library/actions/workflows/chess-library.yml/badge.svg)](https://github.com/Disservin/chess-library/actions/workflows/chess-library.yml)

## [Documentation](https://disservin.github.io/chess-library)

**chess-library** is a multi-purpose library for chess in C++17.

It can be used for any type of chess program, be it a chess engine, a chess GUI, or a chess data anaylsis tool.

### Why this library?

- **Fast**: This library is fast enough for pretty much any purpose in C++ and it is faster than most other chess libraries in C++.
- **Documentation**: Easy to browse **documentation** at <https://disservin.github.io/chess-library>
- **Robust**: Unit Tests & it has been tested on millions of chess positions, while developing the C++ part of [Stockfish's Winrate Model](https://github.com/official-stockfish/WDL_model).
- **PGN Support**: Parse basic PGN files.
- **Namespace**: Everything is in the `chess::` namespace, so it won't pollute your namespace.
- **Compact Board Representation in 24bytes**: The board state can be compressed into 24 bytes, using `PackedBoard` and `Board::Compact::encode`/`Board::Compact::decode`.

> [!NOTE]
> Users are advised to update to the latest version of the library, to fix possible SAN/LAN issues.

### Usage

This is a single header library.

You only need to include `chess.hpp` header!
Aftewards you can access the chess logic over the `chess::` namespace.

### Exceptions

This library might throw exceptions in some cases, for example when the input is invalid or things are not as expected.
To disable exceptions, define `CHESS_NO_EXCEPTIONS` before including the header.

### Benchmarks

Tested on Ryzen 9 5950X.

#### PGN Parser

Ran with `lichess_db_standard_rated_2017-03.pgn` on a Samsung 980 SSD.

| Benchmark | Time    | Throughput    |
| :---:   | :---: | :---: |
| ./build/example | 28.927s   | 413.281 MB/s   |

#### Perft

With movelist preallocation:

| Category | Depth | Time (ms)  | NPS | FEN |
|----------|-------|-----------|-----|-----|
| **Standard Chess** | 7 | 8988 | 355534749 | `rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1` |
| **Standard Chess** | 5 | 430 | 449398352 | `r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1` |
| **Standard Chess** | 7 | 661 | 269839367 | `8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1` |
| **Standard Chess** | 6 | 1683 | 419266646 | `r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1` |
| **Standard Chess** | 5 | 210 | 426261582 | `rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8` |
| **Standard Chess** | 5 | 377 | 434062304 | `r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 1` |
| **Chess960** | 6 | 358 | 331644356 | `rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w HAha - 0 1` |
| **Chess960** | 6 | 710 | 269707784 | `1rqbkrbn/1ppppp1p/1n6/p1N3p1/8/2P4P/PP1PPPP1/1RQBKRBN w FBfb - 0 9`|
| **Chess960** | 6 | 2434 | 379540629 | `rbbqn1kr/pp2p1pp/6n1/2pp1p2/2P4P/P7/BP1PPPP1/R1BQNNKR w HAha - 0 9` |
| **Chess960** | 6 | 927 | 332492639 | `rqbbknr1/1ppp2pp/p5n1/4pp2/P7/1PP5/1Q1PPPPP/R1BBKNRN w GAga - 0 9` |
| **Chess960** | 6 | 2165 | 402734901 | `4rrb1/1kp3b1/1p1p4/pP1Pn2p/5p2/1PR2P2/2P1NB1P/2KR1B2 w D - 0 21` |
| **Chess960** | 6 | 6382 | 419555508 | `1rkr3b/1ppn3p/3pB1n1/6q1/R2P4/4N1P1/1P5P/2KRQ1B1 b Ddb - 0 14` |

### Repositories using this library

- Stockfish Winrate Model
  <https://github.com/official-stockfish/WDL_model>
- CLI Tool for running chess engine matches
  <https://github.com/Disservin/fast-chess>
- GUI-based Chess Player as well as a Chess Engine
  <https://github.com/Orbital-Web/Raphael>
- UCI Chess Engine (\~3.3k elo)
  <https://github.com/rafid-dev/rice> (old version)
- Texel tuner for HCE engines
  <https://github.com/GediminasMasaitis/texel-tuner>

### Development Setup

This project is using the meson build system. <https://mesonbuild.com/>

#### Setup

```bash
meson setup build
```

#### Compilation

```bash
meson compile -C build
```

#### Tests

```bash
meson test -C build
```
or
```bash
meson test -C build --test-args='--test-suite="PGN StreamParser"'
```

#### Example

Download the [Lichess March 2017 database](https://database.lichess.org/standard/lichess_db_standard_rated_2017-03.pgn.zst).
You can decompress this with the following command: `unzstd -d lichess_db_standard_rated_2017-03.pgn.zst`

```bash
cd example
meson setup build
meson compile -C build

./build/example ../lichess_db_standard_rated_2017-03.pgn
```

#### Comparison to other libraries

[Benchmark implementation](./comparison/comparison.md) for more information.
