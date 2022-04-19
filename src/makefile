out: main.o 
	g++ main.o -Wall -o out 

main.o: main.cpp chess.hpp
	g++ -c -flto -O3 -msse2 -march=native -std=c++17 -static-libgcc -static-libstdc++ -Wall main.cpp

clean:
	rm *.o *.exe