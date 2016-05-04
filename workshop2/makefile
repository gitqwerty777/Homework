CC=gcc 
CFLAGS=-std=c99 -g -O2
LDFLAGS=-fopenmp -pthread -lm

main: 1-rand 2-fastMatrixMultiplication 4-prefixSum 5-pthread-omp 5-pthread

1-rand: 1-rand-seq 1-rand-omp

2-fastMatrixMultiplication: matrix-omp.o 2-fastMatrixMultiplication.o
matrix-omp.o: matrix-omp.c matrix.h
	gcc -c -g -O2 -fopenmp -std=c99 matrix-omp.c -o matrix-omp.o

4-prefixSum: 4-prefixSum.c secret.c utils.h

test: test1 test2 test4 test5

test1:
	time ./1-rand-seq
	time ./1-rand-omp
test2:
	time ../workshop1/5-fastMatrixMultiplication < 2input
	time ./2-fastMatrixMultiplication < 2input
test4:
	time ../10084-Prefix-Sum/prefixsum-pthread < 4input
	time ./4-prefixSum < 4input

test5:
	./5-pthread-omp 4 5
	./5-pthread 4 5
clean:
	rm -rf *.o
