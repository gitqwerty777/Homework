main: 
	g++ -fopenmp -O2 fastGameOfLife.cpp -o fast-game-of-life

test: test1 test2 test3 test4

test1:
	time ./fast-game-of-life < testdata1

test2:
	time ./fast-game-of-life < testdata2

test3:
	time ./fast-game-of-life < 3.in > testout
	diff testout 3.out

test4:
	time ./fast-game-of-life < 1.in > testout1
	diff testout1 1.out

