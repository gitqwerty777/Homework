main: program-seq program-parallel test

program-seq: prefixsum-seq.c secret.c utils.h
	gcc -std=c99 -O2 -pthread prefixsum-seq.c secret.c -o prefixsum-seq

program-parallel: prefixsum-pthread.c secret.c utils.h
	gcc -std=c99 -O2 -pthread prefixsum-pthread.c secret.c -o prefixsum-pthread

test:
	time ./prefixsum-seq < testdata
	time ./prefixsum-pthread < testdata
