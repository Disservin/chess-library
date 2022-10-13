out: main.o
	g++ main.o -Wall -o out 

main.o: main.cpp
	g++ -g3 -fno-omit-frame-pointer -DUSE_PEXT -mbmi2 -c -flto -O3 -march=native -std=c++17 -Wall main.cpp

clean:
	rm *.o *.exe