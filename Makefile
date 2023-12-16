CXX = g++
NATIVE = -march=native
CXXFLAGS = -O3 -flto -std=c++17 -Wall -Wextra

# Detect Windows
ifeq ($(OS), Windows_NT)
	uname_S  := Windows
else
	uname_S := $(shell uname -s)
endif

ifeq ($(uname_S), Darwin)
	NATIVE =	
endif

default:
	$(CXX) $(NATIVE) $(CXXFLAGS) -g3 -fno-omit-frame-pointer ./tests/*.cpp -o chess-library-tests

showcase:
	$(CXX) $(NATIVE) $(CXXFLAGS) -g3 -fno-omit-frame-pointer ./example/main.cpp -o chess-library-example

shl:
	python ./tools/shl.py ./src/include.hpp --header_search_paths ./src/ -o ./include/chess.hpp

clean:
	rm *.o *.exe


