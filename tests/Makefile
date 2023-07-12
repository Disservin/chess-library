NATIVE = -march=native
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
	g++ -O3 -flto -DNDEBUG $(NATIVE) -std=c++17 -Wall *.cpp -o out

debug:
	g++ -O3 -flto $(NATIVE) -std=c++17 -g3 -fno-omit-frame-pointer -Wall *.cpp -o out 
	
clean:
	rm *.o *.exe


