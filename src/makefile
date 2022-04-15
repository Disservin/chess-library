out: main.o 
	g++ main.o -o out 

main.o: main.cpp chess.hpp
	g++ -c -flto -O3 -march=native -std=c++17 -static-libgcc -static-libstdc++ main.cpp 

clean:
	del *.o *.exe