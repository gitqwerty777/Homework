#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <omp.h>
#include <cuda.h>
#include "utils.h"
 
#define MAXN 10000005
#define NUM_THREAD 100
uint32_t prefix_sum[MAXN];
uint32_t toAdd[MAXN];
int section[NUM_THREAD+1];
void output(uint32_t presum[], int n) {
  uint32_t hash = 0;
  for (int i = 1; i <= n; i++)
    hash += presum[i] * i;
  printf("%" PRIu32 "\n", hash);
}

__global__ void add(int section[NUM_THREAD+1], uint32_t prefix_sum[MAXN], uint32_t toAdd[NUM_THREAD], int key){
  int index = blockIdx.x+1;
  int start = section[index]+1;
  int end = section[index+1]+1;
  uint32_t sum = 0;
  for (int i = start; i <= end; i++) {
	sum += ((i << (key&31)) | (i >> (32-(key&31))) + key)^key;
	prefix_sum[i] = sum;
  }
  toAdd[index] = prefix_sum[end];
}

__global__ void add2(int section[NUM_THREAD+1], uint32_t prefix_sum[MAXN], uint32_t toAdd[NUM_THREAD], int key){
  int index = blockIdx.x+1;
  int start = section[index]+1;
  int end = section[index+1];
  for (int i = start; i <= end; i++) {
	prefix_sum[i] += toAdd[index];
	prefix_sum[i] *= i;//output
  }
}


int main() {
  int n;
  uint32_t key;

  dim3 grid=dim3(NUM_THREAD,1,1);
  dim3 block=dim3(1,1,1);
  uint32_t* arr_device, *toadd_device;
  int * sec_device;

  cudaMalloc((void**) &arr_device, MAXN*sizeof(uint32_t));
  cudaMalloc((void**) &sec_device, (NUM_THREAD+1)*sizeof(int));
  cudaMalloc((void**) &toadd_device, (NUM_THREAD)*sizeof(uint32_t));
  while (scanf("%d %" PRIu32, &n, &key) == 2) {
    uint32_t sum = 0;
	section[0] = 0;
	section[1] = n/NUM_THREAD;
	for(int i = 2; i < NUM_THREAD; i++){
	  section[i] = section[i-1] + section[1];
	}
	section[NUM_THREAD] = n; // section[3](not include) ~ n

	//cudaMemcpy(arr_device, prefix_sum, MAXN*sizeof(uint32_t), cudaMemcpyHostToDevice);
	cudaMemcpy(sec_device, section, (NUM_THREAD+1)*sizeof(int), cudaMemcpyHostToDevice);
	//

	add<<<grid,block>>>(sec_device, arr_device, toadd_device, key);
	//cudaError_t r=cudaGetLastError();
	//printf("add: %s\n", cudaGetErrorString(r));


	cudaMemcpy(toAdd, toadd_device, MAXN*sizeof(uint32_t), cudaMemcpyDeviceToHost);
	
	for(int i = 1; i < NUM_THREAD; i++){
	  toAdd[i] += toAdd[i-1];
	}
	cudaMemcpy(toadd_device, toAdd, (MAXN)*sizeof(uint32_t), cudaMemcpyHostToDevice);  


   add2<<<grid,block>>>(sec_device, arr_device, toadd_device, key);


   	cudaMemcpy(prefix_sum, arr_device, MAXN*sizeof(uint32_t), cudaMemcpyDeviceToHost);		

    output(prefix_sum, n);
  }

  cudaFree(arr_device);
  cudaFree(sec_device);
  cudaFree(toadd_device);  
  
  return 0;
}
