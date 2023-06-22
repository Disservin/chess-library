## Overview:

A chess library for C++, with reasonably fast move generation for all purposes.  
Also supports FRC/DFRC. Make sure that these are correct and include the file of the rook  
not K/Q/k/q or other annotations. Only a,b,c,d,e,f,g,h and uppercase variants are valid.
Tested on Ryzen 9 5950X:

```
depth 7  time 8088  nodes 3195901860   nps 395092330 fen rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1
depth 5  time 376   nodes 193690690    nps 513768408 fen r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1
depth 7  time 583   nodes 178633661    nps 305879556 fen 8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1
depth 6  time 1396  nodes 706045033    nps 505400882 fen r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1
depth 5  time 185   nodes 89941194     nps 483554806 fen rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8
depth 5  time 330   nodes 164075551    nps 495696528 fen r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 1

averaged:
nodes 4528287989 nps 412976560
```

## Usage

This is a single/two header library.

You only need to include `chess.hpp` header!
Aftewards you can access the chess logic over the `chess::` namespace.
