CXX      := g++
CXXFLAGS := -O3 -flto -march=native -std=c++17 -Wall
FLAGS    := -pthread

# Detect Windows
ifeq ($(OS), Windows_NT)
	uname_S  := Windows
else
ifeq ($(COMP), MINGW)
	uname_S  := Windows
else
	FLAGS   := -pthread
	SUFFIX  :=
	uname_S := $(shell uname -s)
endif
endif

# PGO
ifneq ($(findstring gcc, $(CXX)),)
	PGO_GEN = -fprofile-generate
	PGO_USE = -fprofile-use
else ifneq ($(findstring clang, $(CXX)),)
	PGO_MERGE = llvm-profdata merge -output=profdata *.profraw
	PGO_GEN = -fprofile-instr-generate
	PGO_USE = -fprofile-instr-use=profdata
endif


default:
	$(CXX) $(CXXFLAGS) -DNDEBUG main.cpp -o out

debug:
	$(CXX) $(CXXFLAGS) -fno-omit-frame-pointer main.cpp -o out
	
clean:
	rm *.o *.exe

pgo:
	$(CXX) -DNDEBUG -DPGO_DEPTH=7 $(CXXFLAGS) $(PGO_GEN) main.cpp -o out
	./out
	$(PGO_MERGE)
	$(CXX) -DNDEBUG $(CXXFLAGS) $(PGO_USE) main.cpp -o out