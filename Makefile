CXX = g++
NATIVE = -march=native
CXXFLAGS = -O3 -flto -std=c++17 -Wall -Wextra -DNDEBUG
LDFLAGS =

# Detect Windows
ifeq ($(OS), Windows_NT)
	uname_S  := Windows
else
	uname_S := $(shell uname -s)
endif

ifeq ($(uname_S), Darwin)
	NATIVE =	
endif

# Compile with address sanitizer
ifeq ($(san), asan)
	LDFLAGS += -fsanitize=address
endif

# Compile with memory sanitizer
ifeq ($(san), memory)
	LDFLAGS += -fsanitize=memory -fPIE -pie
endif

# Compile with undefined behavior sanitizer
ifeq ($(san), undefined)
	LDFLAGS += -fsanitize=undefined
endif

default:
	$(CXX) $(NATIVE) $(CXXFLAGS) -g3 -fno-omit-frame-pointer ./tests/*.cpp -o chess-library-tests $(LDFLAGS)

showcase:
	make shl
	$(CXX) $(NATIVE) $(CXXFLAGS) -g3 -fno-omit-frame-pointer ./example/main.cpp -o chess-library-example $(LDFLAGS)

shl:
	python3 ./tools/shl.py ./src/include.hpp --header_search_paths ./src/ -o ./include/chess.hpp

clean:
	rm *.o *.exe


