main: opencl opencl-debug

opencl:
	gcc -std=c99 -O2 gameOfLife-opencl.c -lOpenCL -fopenmp -o main

opencl-debug:
	gcc -std=c99 -O2 -DDEBUG gameOfLife-opencl.c -lOpenCL -fopenmp -o main-debug

reference:
	g++ -std=c99 -O2 reference-openmp.c -fopenmp -o reference

test:
	./main-debug < input1 > tempout
	less tempout

test2:
	./main-debug < input2 > tempout
	less tempout

testans:
	./main < 1.in > tempout
	diff tempout 1.out

testref:
	./reference < 1.in > tempout
	diff tempout 1.out	
