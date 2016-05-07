main: template debug-program

template:
	gcc -std=c99 -O2 initOpenCL.c -lOpenCL -o template

debug-program:
	gcc -std=c99 -O2 OpenCL-Build-Program-Debug.c -lOpenCL -o debug-program

test:
	./debug-program 

