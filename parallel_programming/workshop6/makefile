main: template debug-program debug-program2 debug-program3

template:
	gcc -std=c99 -O2 initOpenCL.c -lOpenCL -o template

debug-program:
	gcc -std=c99 -O2 OpenCL-Build-Program-Debug.c -lOpenCL -o debug-program

debug-program2:
	gcc -std=c99 -O2 -DDEBUG OpenCL-Build-Program-Debug-1024-64.c -lOpenCL -o debug-program2

debug-program3:
	gcc -std=c99 -O2 -DDEBUG OpenCL-Build-Program-Debug-checkinvalid.c -lOpenCL -o debug-program3

test:
	./debug-program 

