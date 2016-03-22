#include <pthread.h>
#include "matrix.h"

#define MAXN 2048
#define NUM_THREAD 4
UINT** A;
UINT** B;
UINT** C;
pthread_mutex_t mutex[MAXN*2];
pthread_t thread[NUM_THREAD];
unsigned long long sum = 0;//overflow

struct Input{
  int x, y;
  Input(int X, int Y){
    x = X;
    y = Y;
  }
};

void* multiply_t(void* input);
void initPthread(){
  for(int i = 0; i < MAXN*2; i++){
    pthread_mutex_init(&mutex[i], NULL); 
  }
  for(int i = 0; i < NUM_THREAD; i++){
    pthread_create(&thread[i], NULL, multiply_t, NULL);/*創建一個線程*/
  }
}

void multiply(int N, unsigned long a[][2048], unsigned long b[][2048], unsigned long c[][2048]) {
  A = a; B = b; C = c;
  initPthread();
  for(int i = 0; i < NUM_THREAD; i++){
    pthread_join(thread[i], NULL);
  }
  for(int i = 0; i < N; i++)
    for(int j = 0; j < N; j++){

    }
}

void* multiply_t(void* input){

}

