all: board.o solver.o
	g++ -std=c++11 -O3 -march=native -Wall solver.o board.o -o solver
solver.o: solver.cpp
	g++ -std=c++11 -Wall -c solver.cpp -o solver.o
board.o: board.h board.cpp
	g++ -std=c++11 -O3 -march=native -Wall -c board.cpp -o board.o
clean:
	rm -f solver *.o
# performance base 1: 0.1sec, 7min, 144min
# performance base 2:
large:
	time ./solver < ../testdata/large.in > large.testout 2> large.stderr
large2:
	time ./solver < ../testdata/large2.in > large2.testout 2>large2.stderr
small:
	time ./solver < ../testdata/small.in > small.testout 2> small.stderr
run:
	time ./solver < ../testdata/small.in > small.testout
	./sokoban -i ../testdata/small.in -o small.testout
check:
	../sokoban/verifier -i ../testdata/small.in -o small.testout
