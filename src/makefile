out: main.o chess.o
	g++ main.o chess.o -Wall -o out 

main.o: main.cpp
	g++ -c -flto -O3 -mcpu=apple-a14 -std=c++17 -Wall main.cpp

chess.o: chess.cpp chess.hpp
	g++ -c -flto -O3 -mcpu=apple-a14 -std=c++17 -Wall chess.cpp

clean:
	rm *.o *.exe