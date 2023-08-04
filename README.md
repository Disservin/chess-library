# An extensive chess library for C++

chess-library is a chess library for C++, with reasonably fast move generation for all purposes, written in c++17.

It can be used for any type of chess program, be it a chess engine, a chess GUI, or a chess data anaylsis tool.

### Why this library?

- **Fast**: This library is fast enough for pretty much any purpose in c++ and it is faster than most other chess libraries in c++.
- **Documentation**: Easy to browse **documentation** at https://disservin.github.io/chess-library
- **Robust**: Unit Tests & it has been tested on millions of chess positions, while developing the C++ part of [Stockfish's Winrate Model](https://github.com/vondele/WLD_model).
- **PGN Support**: Read basic PGN Files.
- **Namespace**: Everything is in the `chess::` namespace, so it won't pollute your namespace.

### Comparison to other chess libraries

The 3 other big chess libraries that I know of in C++ are:

- [surge](https://github.com/nkarve/surge)

  Pros:

  - relatively fast

  Cons:

  - Lacks documentation and utility functionality. Also no support for Chess960.

- [THC](https://github.com/billforsternz/thc-chess-library)

  Pros:

  - Header and source file split, leading to faster compilation times.
  - Position compression

  Cons:

  - Rather slow
  - Lacks documentation
  - No support for Chess960

- [chessgen](https://github.com/markhc/chessgen)

  Pros:

  - Modern C++ (and still active)
  - Header Only

  Cons:

  - No documentation
  - Early Version (after 4 years)
  - No support for Chess960 (I think)

### Repositories using this library:

- Stockfish Winrate Model
  https://github.com/vondele/WLD_model

- CLI Tool for running chess engine matches
  https://github.com/Disservin/fast-chess

- GUI-based Chess Player as well as a Chess Engine
  https://github.com/Orbital-Web/Raphael

- UCI Chess Engine (~3.3k elo)
  https://github.com/rafid-dev/rice (old version)

Tested on Ryzen 9 5950X:

### Standard Chess

```
depth 7  time 8988  nodes 3195901860   nps 355534749 fen rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1
depth 5  time 430   nodes 193690690    nps 449398352 fen r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1
depth 7  time 661   nodes 178633661    nps 269839367 fen 8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1
depth 6  time 1683  nodes 706045033    nps 419266646 fen r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1
depth 5  time 210   nodes 89941194     nps 426261582 fen rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8
depth 5  time 377   nodes 164075551    nps 434062304 fen r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 1
```

### Chess960

```
depth 6  time 358   nodes 119060324    nps 331644356 fen rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w HAha - 0 1
depth 6  time 710   nodes 191762235    nps 269707784 fen 1rqbkrbn/1ppppp1p/1n6/p1N3p1/8/2P4P/PP1PPPP1/1RQBKRBN w FBfb - 0 9
depth 6  time 2434  nodes 924181432    nps 379540629 fen rbbqn1kr/pp2p1pp/6n1/2pp1p2/2P4P/P7/BP1PPPP1/R1BQNNKR w HAha - 0 9
depth 6  time 927   nodes 308553169    nps 332492639 fen rqbbknr1/1ppp2pp/p5n1/4pp2/P7/1PP5/1Q1PPPPP/R1BBKNRN w GAga - 0 9
depth 6  time 2165  nodes 872323796    nps 402734901 fen 4rrb1/1kp3b1/1p1p4/pP1Pn2p/5p2/1PR2P2/2P1NB1P/2KR1B2 w D - 0 21
depth 6  time 6382  nodes 2678022813   nps 419555508 fen 1rkr3b/1ppn3p/3pB1n1/6q1/R2P4/4N1P1/1P5P/2KRQ1B1 b Ddb - 0 14
```

### Usage

This is a single header library.

You only need to include `chess.hpp` header!
Aftewards you can access the chess logic over the `chess::` namespace.
