out: main.o chess.o
	g++ main.o chess.o -Wall -o out 

main.o: main.cpp
	g++ -c -flto -O3 -msse2 -march=native -std=c++17 -static-libgcc -static-libstdc++ -Wall main.cpp

chess.o: chess.cpp chess.hpp
	g++ -c -flto -O3 -msse2 -march=native -std=c++17 -static-libgcc -static-libstdc++ -Wall chess.cpp

clean:
	rm *.o *.exe