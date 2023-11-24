#include <pthread.h>
#include "matrix.h"
#include <stdio.h>

#define MAXN 2048
#define NUM_THREAD 4
#define UINT unsigned long 
UINT A[MAXN][MAXN];
UINT B[MAXN][MAXN];
UINT C[MAXN][MAXN];
int N;
int isComplete[MAXN*2];
pthread_mutex_t mutex[MAXN*2];
pthread_t thread[NUM_THREAD];
unsigned long long sum = 0;//overflow
void* multiply_t(void* input);

void initPthread(){
  for(int i = 0; i < N*2; i++){
    pthread_mutex_init(&mutex[i], NULL); 
  }
  for(int i = 0; i < NUM_THREAD; i++){
    pthread_create(&thread[i], NULL, multiply_t, (void*)i);
  }
}

void multiply(int n, unsigned long a[][2048], unsigned long b[][2048], unsigned long c[][2048]) {
  //A = a; B = b; C = c;
  for(int i = 0; i < N; i++){
    memcpy(A[i], a[i], sizeof(int)*N);
    memcpy(B[i], b[i], sizeof(int)*N);
    memcpy(C[i], c[i], sizeof(int)*N);
  }
  
  N = n;
  
  for(int i = 0; i < N*2; i++){
    isComplete[i] = 0;
  }
  initPthread();
  for(int i = 0; i < NUM_THREAD; i++){
    pthread_join(thread[i], NULL);
  }
  for(int i = 0; i < N; i++)
    memcpy(C[i], c[i], sizeof(int)*N);
}

void calculateRow(int row){
  for(int c = 0; c < N; c++){
    //C[row*2048+c] = 0;
    C[row][c] = 0;
    for(int k = 0; k < N; k++)
      //C[row*2048+c] += A[row*2048+k] * B[k*2048+c];
      C[row][c] += A[row][k] * B[k][c];
  }
}

void* multiply_t(void* input){
  for(int r = 0; r < N; r++){
    if(r % NUM_THREAD == (int)input){
    //if((isComplete[r] == 0) && (pthread_mutex_trylock(&mutex[r]) == 0)){//do calculation//TODO: check = 0  -> = 1
      calculateRow(r);
      isComplete[r] = 1;
      pthread_mutex_unlock(&mutex[r]);
    } //else continue
  }
  pthread_exit(0);
}

