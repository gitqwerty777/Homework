main: original new

original:
	g++ 10027-Fast-Sudoku.cpp -std=c++98 -fopenmp -O2 -o sudoku

new:
	g++ Fast-Sudoku.cpp -std=c++98 -fopenmp -O2 -o fast-sudoku

test:
	./fast-sudoku < testdata1
	time ./fast-sudoku < testdata2
	time ./fast-sudoku < testdata3
	time ./fast-sudoku < testdata4
	time ./fast-sudoku < testdata5
	time ./sudoku < testdata2
	time ./sudoku < testdata3
	time ./sudoku < testdata4
	time ./sudoku < testdata5


