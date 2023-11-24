#include <omp.h>
#include "matrix.h"
#include <stdio.h>

#define MAXN 2048
#define NUM_THREAD 4
#define UINT unsigned long 

unsigned long long sum = 0;//overflow

void multiply(int n, unsigned long A[][2048], unsigned long B[][2048], unsigned long C[][2048]) {
  
  omp_set_num_threads(NUM_THREAD);
  #pragma omp parallel for  
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      unsigned long sum = 0;    // overflow, let it go.
      for (int k = 0; k < n; k++)
	sum += A[i][k] * B[k][j];
      C[i][j] = sum;
    }
  }
}




