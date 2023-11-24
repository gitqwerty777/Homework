main: program-seq program-parallel program-parallel-omp test

program-seq: prefixsum-seq.c secret.c utils.h
	gcc -std=c99 -O2 -pthread prefixsum-seq.c secret.c -o prefixsum-seq

program-parallel: prefixsum-pthread.c secret.c utils.h
	gcc -std=gnu99 -O2 -pthread prefixsum-pthread.c secret.c -o prefixsum-pthread

program-parallel-omp: prefixsum-omp.c secret.c utils.h
	gcc -std=c99 -O2 -fopenmp prefixsum-omp.c secret.c -o prefixsum-omp

test:
	time ./prefixsum-seq < testdata
	time ./prefixsum-pthread < testdata
	time ./prefixsum-omp < testdata
