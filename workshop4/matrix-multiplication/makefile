main: parallel

test:
	./matrix-mul-parallel <input1
test2:
	./matrix-mul-parallel <input2

sequential:
	g++ matrix-mul-sequential.cpp -O2 -o ./matrix-mul-sequential

parallel:
	g++ -g matrix-mul-parallel.cpp -O2 -o ./matrix-mul-parallel -fopenmp 
