all:
	g++ pigeonhole.cpp -o pigeonhole

run:
	./pigeonhole 3 3 > pigeonhole33
	./pigeonhole 3 4 > pigeonhole34

test:
	minisat pigeonhole33; minisat pigeonhole34 
