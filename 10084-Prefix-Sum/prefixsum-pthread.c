#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <inttypes.h>
#include "utils.h"
 
#define MAXN 10000005
#define MAX_THREAD 4
uint32_t prefix_sum[MAXN];
int section[5];
pthread_t tid[4];

int n;
uint32_t key;

void *encrypt_t (void *param);

int main() {
  while (scanf("%d %" PRIu32, &n, &key) == 2) {

    if(n >= 10000){

      section[0] = 0; 
      section[1] = n>>2;// 0(not include) ~ n/4
      section[2] = n>>1;
      section[3] = section[1]+section[2];
      section[4] = n; // section[3](not include) ~ n
      
      pthread_create(&tid[0], NULL, encrypt_t, (void*)(0));/*創建一個線程*/
      pthread_create(&tid[1], NULL, encrypt_t, (void*)(1));
      pthread_create(&tid[2], NULL, encrypt_t, (void*)(2));
      pthread_create(&tid[3], NULL, encrypt_t, (void*)(3));

      pthread_join(tid[0], NULL);/*等待子線程執行完畢*/
      pthread_join(tid[1], NULL);
      pthread_join(tid[2], NULL);
      pthread_join(tid[3], NULL);

      for(int p = 1; p < 4; p++){
	uint32_t sum = prefix_sum[section[p]];
	for(int i = section[p]+1; i <= section[p+1]; i++){
	  prefix_sum[i] += sum;
	}
      }
      
    } else {

      uint32_t sum = 0;
      for (int i = 1; i <= n; i++) {
	sum += encrypt(i, key);
	prefix_sum[i] = sum;
      }
      
    }
    
    output(prefix_sum, n);
  }
  return 0;
}

void *encrypt_t (void *param){
  int index = (int*)param;
  int start = section[index]+1;
  int end = section[index+1];

  uint32_t sum = 0;
  for (int i = start; i <= end; i++) {
    sum += encrypt(i, key);
    prefix_sum[i] = sum;
  }
  pthread_exit(0);
}
