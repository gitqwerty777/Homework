main: opencl sequential

devices1: 
	gcc -std=c99 -DN=1024 -DDEVICENUM=1 -O2 matrixMul-time-copy-local-multidevice.c -o matrix-opencl1 -lm -lOpenCL -fopenmp -I/usr/include/CL

devices2: 
	gcc -std=c99 -DN=1024 -DDEVICENUM=2 -O2 matrixMul-time-copy-local-multidevice.c -o matrix-opencl2 -lm -lOpenCL -fopenmp -I/usr/include/CL

devicegc:
	gcc -std=c99 -DN=1024 -DDEVICENUM=2 -O2 matrixMul-time-copy-local-multidevice-gc.c -o matrix-openclgc -lm -lOpenCL -fopenmp -I/usr/include/CL

devicesxy:
	gcc -std=c99  -DDEVICENUM=2 -O2 matrixCalculator-opencl.c -o matrix-openclxy -lm -lOpenCL -fopenmp -I/usr/include/CL

opencl:
	gcc -std=c99 -O2 matrix-opencl.c -o matrix-opencl -lm -lOpenCL -fopenmp -I/usr/include/CL 

sequential:
	gcc -std=c99 -O2 matrix-sequential.c -lm -o matrix-sequential

test:
	time ./matrix-opencl < input2
	time ./matrix-sequential < input2


