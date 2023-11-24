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

__global__ void add(int inputs[100], uint32_t outputs[100]){
  int index = blockIdx.x;
  uint32_t key = inputs[index];
  uint32_t sum = 0;
  uint32_t hash = 0;  
  for (int i = 1; i <= MAXN; i++) {
	sum += ((i << (key&31)) | (i >> (32-(key&31))) + key)^key;
    hash += sum * i;	
  }
  outputs[index] = hash;
}

int inputs[100];
uint32_t outputs[100];

int main() {
  int n;
  uint32_t key;

  dim3 grid=dim3(100,1,1);
  dim3 block=dim3(1,1,1);
  int * input_device;
  uint32_t* output_device;

  cudaMalloc((void**) &input_device, 100*sizeof(int));
  cudaMalloc((void**) &output_device, 100*sizeof(uint32_t));

  int inputcount = 0;
  while (scanf("%d %" PRIu32, &n, &key) == 2) {
	inputs[inputcount++] = key;
  }

  uint32_t sum = 0;
  cudaMemcpy(input_device, inputs, (100)*sizeof(int), cudaMemcpyHostToDevice);
  cudaMemcpy(output_device, outputs, (100)*sizeof(uint32_t), cudaMemcpyHostToDevice);  
  add<<<grid,block>>>(input_device, output_device);
  cudaError_t r=cudaGetLastError();
  cudaMemcpy(outputs, output_device, (100)*sizeof(uint32_t), cudaMemcpyDeviceToHost);
  cudaMemcpy(inputs, input_device, (100)*sizeof(int), cudaMemcpyDeviceToHost);

  for(int i = 0; i < 100; i++){
	printf("%u\n", outputs[i]);
  }

  cudaFree(input_device);
  cudaFree(output_device);

  return 0;
}
