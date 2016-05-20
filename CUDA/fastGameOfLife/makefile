main: cuda

cuda:
	nvcc -Xcompiler "-O2 -fopenmp" game-of-life.cu -o main

test:
	./main-debug < input1 > tempout
	less tempout

test2:
	./main-debug < input2 > tempout
	less tempout

testans:
	./main < 1.in > tempout
	diff --brief tempout 1.out

testans3:
	./main < 3.in > tempout
	diff --brief tempout 3.out

testans2:
	./main < 5.in > tempout
	diff --brief tempout 5.out

testref:
	./reference < 1.in > tempout
	diff tempout 1.out	
