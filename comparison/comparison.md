# Comparison to other chess libraries

## PERFT Comparison

[Benchmark implementation](./comparison/benchmark.cpp) for more information.

`chess-library` had no movelist preallocation, same as the other libraries.

```
chess-library:
depth 6  time 539   nodes 119060324    nps 220482081 fen rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1
depth 5  time 538   nodes 193690690    nps 359351929 fen r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1
depth 6  time 64    nodes 11030083     nps 169693584 fen 8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1
depth 5  time 53    nodes 15833292     nps 293209111 fen r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1
depth 5  time 267   nodes 89941194     nps 335601470 fen rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8
depth 5  time 468   nodes 164075551    nps 349841260 fen r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 11

Surge:
depth 6  time 713   nodes 119060324    nps 166751154 fen rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1
depth 5  time 841   nodes 193690690    nps 230036448 fen r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1
depth 6  time 75    nodes 11030083     nps 145132671 fen 8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1
depth 5  time 85    nodes 15833292     nps 184108046 fen r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1
depth 5  time 419   nodes 89941194     nps 214145700 fen rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8
depth 5  time 770   nodes 164075551    nps 212808756 fen r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 11

THC:
depth 6  time 3294  nodes 119060324    nps 36133633  fen rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1
depth 5  time 5043  nodes 193690690    nps 38400216  fen r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1
depth 6  time 404   nodes 11030083     nps 27234772  fen 8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1
depth 5  time 388   nodes 15833292     nps 40702550  fen r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1
depth 5  time 2909  nodes 89941194     nps 30907626  fen rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8
depth 5  time 3403  nodes 164075551    nps 48200808  fen r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 11
```

## Features

The 3 other big chess libraries that I know of in C++ are:

- [surge](https://github.com/nkarve/surge)

  Pros:

  - relatively fast, see [PERFT Comparison](#perft-comparison)

  Cons:

  - lacks documentation and utility functionality, also no support for Chess960.
  - not very active anymore

- [THC](https://github.com/billforsternz/thc-chess-library)

  Pros:

  - Header and source file split, leading to faster compilation times.
  - Position compression

  Cons:

  - Rather slow, see [PERFT Comparison](#perft-comparison)
  - Lacks documentation
  - No support for Chess960

- [chessgen](https://github.com/markhc/chessgen)

  Pros:

  - Modern C++ (and relatively active)
  - Header Only

  Cons:

  - No documentation
  - Early Version (after 4 years)
  - No support for Chess960 (I think)
