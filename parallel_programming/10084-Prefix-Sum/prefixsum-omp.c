#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <omp.h>
#include "utils.h"
 
#define MAXN 10000005
#define NUM_THREAD 4
uint32_t prefix_sum[MAXN];
int section[NUM_THREAD+1];
int main() {
  int n;
  uint32_t key;
  while (scanf("%d %" PRIu32, &n, &key) == 2) {
    section[0] = 0;
    section[1] = n/NUM_THREAD;
    for(int i = 2; i < NUM_THREAD; i++){
      section[i] = section[i-1] + section[1];
    }
    section[NUM_THREAD] = n; // section[3](not include) ~ n

    omp_set_num_threads(NUM_THREAD);
    uint32_t toAdd[NUM_THREAD] = {0};
#pragma omp parallel for 
    for (int index = 0; index < NUM_THREAD; index++) {
      int start = section[index]+1;
      int end = section[index+1];
      uint32_t sum = 0;
      for (int i = start; i <= end; i++) {
	sum += encrypt(i, key);
	prefix_sum[i] = sum;
      }
      toAdd[index] = prefix_sum[end];
    }
    for(int i = 1; i < NUM_THREAD; i++){
      toAdd[i] += toAdd[i-1];
    }
#pragma omp parallel for 
    for (int index = 1; index < NUM_THREAD; index++) {
      int start = section[index]+1;
      int end = section[index+1];
      for (int i = start; i <= end; i++) {
	prefix_sum[i] += toAdd[index-1];
      }
      //fprintf(stderr, "total[%d] = %d\n", end, prefix_sum[end]);
    }
    output(prefix_sum, n);
  }
  return 0;
}
