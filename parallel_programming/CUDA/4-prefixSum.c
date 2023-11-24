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
    uint32_t sum = 0;
	uint32_t hash = 0;

	for (int i = 1; i <= n; i++) {
	  sum += ((i << (key&31)) | (i >> (32-(key&31))) + key)^key;
	  hash += sum * i;	
	}
	
	printf("%u\n", hash);
  }
  return 0;
}
