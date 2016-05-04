CC=gcc 
CFLAGS=-std=c99 -g -O2
LDFLAGS=-pthread -fopenmp -lm

main: matrix-seq matrix-pthread matrix-omp

matrix-seq: fastMatrixMultiplication.o matrix-seq.o
matrix-pthread: fastMatrixMultiplication.o matrix-pthread.o
matrix-omp: fastMatrixMultiplication.o matrix-omp.o

fastMatrixMultiplication.o: fastMatrixMultiplication.c matrix.h
matrix-seq.o: matrix-seq.c matrix.h
matrix-pthread.o: matrix-pthread.c matrix.h
matrix-omp.o: matrix-omp.c matrix.h

test:
	time ./matrix-seq < input
	time ./matrix-pthread < input
	time ./matrix-omp < input
