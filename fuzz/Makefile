CXX=clang++

CXXFLAGS=-std=c++17 -g -O1 -fsanitize=fuzzer,address

all:
	$(CXX) $(CXXFLAGS) -o pgn_fuzzer pgn_fuzzer.cpp;

fuzz:
	./pgn_fuzzer corpus

clean:
	rm -f pgn_fuzzer
	rm -f *.o
	rm -f

.PHONY: all clean

