main: opencl sequential

opencl:
	gcc -std=c99 -O2 matrix-opencl.c -o matrix-opencl -lm -lOpenCL -fopenmp -I/usr/include/CL 

sequential:
	gcc -std=c99 -O2 matrix-sequential.c -lm -o matrix-sequential

test:
	time ./matrix-opencl < input2
	time ./matrix-sequential < input2


