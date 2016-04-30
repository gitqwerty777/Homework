main: parallel sequential

test:
	time ./matrix-mul-sequential <input1
	time ./matrix-mul-parallel <input1
test2:
	time ./matrix-mul-parallel <input2
	time ./matrix-mul-sequential <input2

sequential:
	g++ matrix-mul-parallel.cpp -O2 -o ./matrix-mul-sequential 

parallel:
	g++ -g matrix-mul-parallel.cpp -O2 -o ./matrix-mul-parallel -fopenmp 
