#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <inttypes.h>
#include "utils.h"
 
#define MAXN 10000005
#define MAX_THREAD 4
uint32_t prefix_sum[MAXN];
uint32_t arr[MAXN];
pthread_t tid[4];

int n;
uint32_t key;

void *encrypt_t (void *param);

int main() {
  while (scanf("%d %" PRIu32, &n, &key) == 2) {

    pthread_create(&tid[0], NULL, encrypt_t, (void*)(0));/*創建一個線程*/
    pthread_create(&tid[1], NULL, encrypt_t, (void*)(1));
    pthread_create(&tid[2], NULL, encrypt_t, (void*)(2));
    pthread_create(&tid[3], NULL, encrypt_t, (void*)(3));

    pthread_join(tid[0], NULL);/*等待子線程執行完畢*/
    pthread_join(tid[1], NULL);
    pthread_join(tid[2], NULL);
    pthread_join(tid[3], NULL);

    prefix_sum[1] = arr[1];
    for(int i = 2; i <= n; i++){
      prefix_sum[i] = prefix_sum[i-1] + arr[i];
    }
    
    output(prefix_sum, n);
  }
  return 0;
}

void *encrypt_t (void *param){
  int index = (int*)param;
  int i = 1;
  while((i % 4) != index && i <= n){
    i++;
  }
  for (; i <= n; i += 4) {
    arr[i] = encrypt(i, key);
  }  
  pthread_exit(0);
}
