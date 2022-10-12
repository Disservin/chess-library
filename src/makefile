out: main.o
	g++ main.o -Wall -o out 

main.o: main.cpp
	g++ -c -flto -O3 -funroll-loops -march=native -std=c++17 -Wall main.cpp

clean:
	rm *.o *.exe