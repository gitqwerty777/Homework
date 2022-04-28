CC = gcc -std=c11 -O2 -Wall
DEL = rm -f
all:
	$(CC) *.c -c
	$(CC) game.o getch.o sokoban.o -o game
	$(CC) sokoban.o verifier.o -o verifier
	$(DEL) -f *.o
clean:
	$(DEL) -f game verifier
