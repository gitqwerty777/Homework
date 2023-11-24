all: merge merge-nvcc

testsync:
	nvcc 3-testsync.cu -o testsync

average-global:
	nvcc 5-average-global.cu -o average-global

average:
	nvcc 5-average.cu -o average

prefix-original:
	gcc -g -O2 -std=c99 4-prefixSum.c secret.c -o prefix-original

prefix: prefixsum-pp.cu
	nvcc prefixsum-pp.cu -o prefix

lib:
	g++ liberty.cpp -o lib-seq

exceed:
	nvcc exceedSharedMemory.cu -o exceedSharedMemory

deviceinfo:
	nvcc deviceInfo.cu -o deviceInfo

merge-nvcc:
	nvcc merge.cu -o main

merge:
	g++ merge.cpp -o main -L/usr/local/cuda/lib64 -lcuda -lcudart

seperate: first_cuda.o
	g++ main.cpp first_cuda.o -o main -L/usr/local/cuda/lib64 -lcuda -lcudart

first_cuda.o:
	nvcc -c first_cuda.cu 

clean:
	rm -rf *.o
