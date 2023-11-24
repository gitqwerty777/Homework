
CFLAG=-std=c99 -pthread

all: old new

old: main.c sum.c job.c
	gcc $(CFLAG) main.c -c 
	gcc $(CFLAG) sum.c -c  
	gcc $(CFLAG) main.o sum.o job.c -o job

new: main.c sum.c newjob.c
	gcc $(CFLAG) main.c -c 
	gcc $(CFLAG) sum.c -c  
	gcc $(CFLAG) main.o sum.o newjob.c -o newjob

test:
	time ./job < input
	time ./newjob < input
