#include <stdio.h>
#include <assert.h>
#include <omp.h>
#include <inttypes.h>
#include <CL/cl.h>
#include "utils.h"
 
#define MAXGPU 8
#define MAXCODESZ 32767
#define MAXN 16777216
static cl_uint A[MAXN], B[MAXN], C[MAXN];
int main(int argc, char *argv[]) {
  omp_set_num_threads(4);
  int N;
  uint32_t key1, key2;
  while (scanf("%d %" PRIu32 " %" PRIu32, &N, &key1, &key2) == 3) {
    int chunk = N / 4;
    for (int i = 0; i < N; i++) {
      A[i] = encrypt(i, key1);
      B[i] = encrypt(i, key2);
    }
 
    for (int i = 0; i < N; i++)
      C[i] = A[i] * B[i];
 
    uint32_t sum = 0;
    for (int i = 0; i < N; i++)
      sum += C[i];
    printf("%" PRIu32 "\n", sum);
  }
  return 0;
}
