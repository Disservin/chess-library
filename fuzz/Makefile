CXX=clang++

CXXFLAGS=-std=c++17 -g -O1 -fsanitize=fuzzer,address

all:
	$(CXX) $(CXXFLAGS) -o pgn_fuzzer pgn_fuzzer.cpp;
	$(CXX) $(CXXFLAGS) -o fen_fuzzer fen_fuzzer.cpp;

fuzz:
	./pgn_fuzzer corpus

fuzz-fen:
	./fen_fuzzer corpus-fen

clean:
	rm -f pgn_fuzzer fen_fuzzer
	rm -f *.o
	rm -f

.PHONY: all clean

