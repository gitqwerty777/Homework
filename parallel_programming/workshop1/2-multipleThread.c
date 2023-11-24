#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <inttypes.h>

#define MAX_THREAD 4
pthread_t tid[MAX_THREAD];
void *encrypt_t (void *param);
int main() {
  for(int i = 0; i < MAX_THREAD; i++){
    pthread_create(&tid[i], NULL, encrypt_t, (void*)(i));/*創建一個線程*/
    pthread_join(tid[i], NULL);
  }
  return 0;
}

void *encrypt_t (void *param){
  int index = (int*)param;
  fprintf(stderr, "input = %d\n", index);
  pthread_exit(0);
}
