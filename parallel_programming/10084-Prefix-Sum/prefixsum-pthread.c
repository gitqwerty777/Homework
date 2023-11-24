#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <inttypes.h>
#include "utils.h"
 
#define MAXN 10000005
#define MAX_THREAD 4
uint32_t prefix_sum[MAXN];
int section[MAX_THREAD+1];
pthread_t tid[MAX_THREAD];
pthread_mutex_t mutex[MAX_THREAD];
pthread_barrier_t barrier;
int n;
uint32_t key;

void *encrypt_t (void *param);

int main() {
  for (int i = 0; i < MAX_THREAD; i++){
    pthread_mutex_init(&mutex[i], NULL);
  }
  pthread_barrier_init(&barrier, NULL, (unsigned)MAX_THREAD);
  
  while (scanf("%d %" PRIu32, &n, &key) == 2) {
    if(n >= 4){
      section[0] = 0;
      section[1] = n/MAX_THREAD;
      for(int i = 2; i < MAX_THREAD; i++){
	section[i] = section[i-1] + section[1];
      }
      section[MAX_THREAD] = n; // section[3](not include) ~ n
      
      for(int i = 0; i < MAX_THREAD; i++){
	pthread_create(&tid[i], NULL, encrypt_t, (void*)(i));/*創建一個線程*/
      }
      for(int i = 0; i < MAX_THREAD; i++){
	pthread_join(tid[i], NULL);
      }
    } else {//original
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
  pthread_mutex_lock(&mutex[index]);
  pthread_barrier_wait(&barrier);
  int start = section[index]+1;
  int end = section[index+1];

  uint32_t sum = 0;
  for (int i = start; i <= end; i++) {
    sum += encrypt(i, key);
    prefix_sum[i] = sum;
  }
  if(index == 0){
    pthread_mutex_unlock(&mutex[index]);
    pthread_exit(0);
  }
  pthread_mutex_lock(&mutex[index-1]);
  prefix_sum[end] += prefix_sum[start-1];
  //fprintf(stderr, "total[%d] = %d\n", end, prefix_sum[end]);
  pthread_mutex_unlock(&mutex[index-1]);
  pthread_mutex_unlock(&mutex[index]);

  //start to add prefix calculated by previous thread
  for(int i = start; i < end; i++)//not include end
    prefix_sum[i] += prefix_sum[start-1];
  pthread_exit(0);
}
