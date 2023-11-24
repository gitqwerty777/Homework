main: sequential opencl

sequential:
	gcc -std=c99 -O2 main.c -fopenmp
opencl:
	gcc -std=c99 -O2 main-opencl.c -lOpenCL -fopenmp
