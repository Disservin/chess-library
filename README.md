# [Documentation](https://disservin.github.io/chess-library/)

### Overview:

A chess library for C++, with reasonably fast move generation for all purposes.  
Also supports FRC/DFRC. Make sure that these are correct and include the file of the rook  
not K/Q/k/q or other annotations. Only a,b,c,d,e,f,g,h and uppercase variants are valid.
Tested on Ryzen 9 5950X:

```
depth 7  time 9893  nodes 3195901860   nps 323014135 fen rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1
depth 5  time 489   nodes 193690690    nps 395287122 fen r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1
depth 7  time 665   nodes 178633661    nps 268218710 fen 8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1
depth 6  time 1824  nodes 706045033    nps 386873990 fen r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1
depth 5  time 236   nodes 89941194     nps 379498708 fen rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8
depth 5  time 424   nodes 164075551    nps 386060120 fen r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 1

averaged:
nodes 4528287989 nps 334462514

Starting Chess960 tests
depth 6  time 393   nodes 119060324    nps 302183563 fen rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w HAha - 0 1
depth 6  time 715   nodes 191762235    nps 267824350 fen 1rqbkrbn/1ppppp1p/1n6/p1N3p1/8/2P4P/PP1PPPP1/1RQBKRBN w FBfb - 0 9
depth 6  time 2584  nodes 924181432    nps 357516994 fen rbbqn1kr/pp2p1pp/6n1/2pp1p2/2P4P/P7/BP1PPPP1/R1BQNNKR w HAha - 0 9
depth 6  time 956   nodes 308553169    nps 322417104 fen rqbbknr1/1ppp2pp/p5n1/4pp2/P7/1PP5/1Q1PPPPP/R1BBKNRN w GAga - 0 9
depth 6  time 2428  nodes 872323796    nps 359128775 fen 4rrb1/1kp3b1/1p1p4/pP1Pn2p/5p2/1PR2P2/2P1NB1P/2KR1B2 w D - 0 21
depth 6  time 7055  nodes 2678022813   nps 379538380 fen 1rkr3b/1ppn3p/3pB1n1/6q1/R2P4/4N1P1/1P5P/2KRQ1B1 b Ddb - 0 14

averaged:
nodes 5093903769 nps 360349728
```

### Usage

This is a single header library.

You only need to include `chess.hpp` header!
Aftewards you can access the chess logic over the `chess::` namespace.
