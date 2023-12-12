EXE = out
CXX = g++

NATIVE = -march=native

CXXFLAGS = -O3 -flto -std=c++20 -Wall -Wextra


# Detect Windows
ifeq ($(OS), Windows_NT)
	uname_S  := Windows
else
ifeq ($(COMP), MINGW)
	uname_S  := Windows
else
	uname_S := $(shell uname -s)
endif
endif

# Different native flag for macOS
ifeq ($(uname_S), Darwin)
	NATIVE =	
endif

default:
	$(CXX) $(NATIVE) $(CXXFLAGS) *.cpp -o $(EXE)

debug:
	$(CXX) $(NATIVE) $(CXXFLAGS) -g3 -fno-omit-frame-pointer *.cpp -o $(EXE)
	
clean:
	rm *.o *.exe


