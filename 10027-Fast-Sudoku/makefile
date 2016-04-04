main: original new

original:
	g++ 10027-Fast-Sudoku.cpp -std=c++98 -fopenmp -O2 -o sudoku

new:
	g++ Fast-Sudoku.cpp -std=c++98 -fopenmp -O2 -o fast-sudoku

test:
	time ./sudoku < testdata2
	./fast-sudoku < testdata1
	time ./fast-sudoku < testdata2

