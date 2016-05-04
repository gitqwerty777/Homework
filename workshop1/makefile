CC=gcc 
CFLAGS=-std=c99 -g -O2
LDFLAGS=-pthread -lm

main: 1-singleThead 2-multipleThread 3-receiveThreadReturn 4-matrix-seq 5-fastMatrixMultiplication

4-matrix-seq: matrix.o 4-matrix-seq.o
5-fastMatrixMultiplication: matrix-pthread.o 5-fastMatrixMultiplication.o

matrix.o: matrix-seq.c matrix.h
	gcc -c -g -O2 -pthread -std=c99 matrix-seq.c -o matrix.o
matrix-pthread.o: matrix-pthread.c matrix.h
	gcc -c -g -O2 -pthread -std=c99 matrix-pthread.c -o matrix-pthread.o

test: test1 test2 test3 test4 test5

test1:
	./1-singleThead
test2:
	./2-multipleThread
test3:
	./3-receiveThreadReturn
test4:
	time ./4-matrix-seq < 4input
test5:
	time ./5-fastMatrixMultiplication < 4input

clean:
	rm -rf *.o
