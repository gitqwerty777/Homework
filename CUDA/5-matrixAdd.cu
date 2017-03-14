#include <stdio.h>
#include <stdlib.h>
#include <cuda.h>

#define MAXN 1024
int arr[MAXN];

__global__ void add(int A[MAXN][MAXN], int B[MAXN][MAXN]){
  int i = blockDim.x*blockIdx.x+threadIdx.x+1;
  int j = blockDim.y*blockIdx.y+threadIdx.y+1;
  int li = threadIdx.x+1;
  int lj = threadIdx.y+1;
  __shared__ unsigned int sur[BSIDE+2][BSIDE+2];//surroundings

  sur[li][lj] = arr[i][j];
  
  
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
  cudaMalloc((void**) &toadd_device, (MAXN)*sizeof(uint32_t));
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
	//cudaMemcpy(toadd_device, toAdd, (MAXN)*sizeof(uint32_t), cudaMemcpyHostToDevice);  
  
	add<<<grid,block>>>(sec_device, arr_device, toadd_device, key);
	cudaError_t r=cudaGetLastError();
	printf("add: %s\n", cudaGetErrorString(r));

	cudaMemcpy(prefix_sum, arr_device, MAXN*sizeof(uint32_t), cudaMemcpyDeviceToHost);		
	
	cudaFree(arr_device);
  
	return 0;
  }
