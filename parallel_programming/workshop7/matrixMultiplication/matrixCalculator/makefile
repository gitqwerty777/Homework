main: opencl opencl-debug

opencl: matrixCalculator-opencl.c matrix-lib.cl
	gcc -std=c99 -O2 matrixCalculator-opencl.c -o main -lm -lOpenCL -fopenmp

opencl-debug: matrixCalculator-opencl.c matrix-lib.cl
	gcc -std=c99 -O2 -DDEBUG matrixCalculator-opencl.c -o main-debug -lm -lOpenCL -fopenmp

test:

