#include <stdio.h>
#include <stdlib.h>
#include <cuda.h>
#include "cuda_runtime.h"

#define MAXN 1024
int A[MAXN], B[MAXN], C[MAXN];

__global__ void addone(int arr[MAXN]){
  int li = threadIdx.x;
  int index = blockIdx.x*4+li;  

  __shared__ int sharedint[4];

  sharedint[li] = arr[index];
  __syncthreads();
  if(li == 0)
	for(int i = 0; i < 4; i++)
	  sharedint[i] += 1;
  __syncthreads();  
  arr[index] = sharedint[li];
}

__global__ void vectorAdd(int A[MAXN], int B[MAXN], int C[MAXN]){
  int index = blockIdx.x;
  C[index] = A[index]+B[index];
}

dim3 grid1=dim3(MAXN/4,1,1);
dim3 block1=dim3(4,1,1);
dim3 grid=dim3(MAXN,1,1);
dim3 block=dim3(1,1,1);
cudaStream_t stream1, stream0, stream2;

void doAddone(int devicei, int* a, cudaStream_t* stream){
  cudaSetDevice(devicei);
  addone<<<grid1,block1, 0, *stream>>>(a);
  //cudaError_t r=cudaGetLastError();
  //printf("addone : %s\n", cudaGetErrorString(r));	
}

void doAdd(int devicei, int *a, int *b , int *c){
  cudaSetDevice(devicei);
  vectorAdd<<<grid,block, 0, stream2>>>(a, b, c);
  //cudaError_t r=cudaGetLastError();
  //printf("add: %s\n", cudaGetErrorString(r));	
}

int main() {
  int numDevs = 0;
  cudaGetDeviceCount(&numDevs);
  printf("total %d devices\n", numDevs);

  int *a, *b, *c, *aa, *ba, *ca;

  cudaSetDevice(0);
  cudaMalloc((void**) &a, MAXN*sizeof(int));
  cudaMalloc((void**) &b, MAXN*sizeof(int));
  cudaMalloc((void**) &c, MAXN*sizeof(int));

  cudaMemcpy(a, A, MAXN*sizeof(int), cudaMemcpyHostToDevice);
  cudaMemcpy(b, B, MAXN*sizeof(int), cudaMemcpyHostToDevice);
  cudaMemcpy(c, C, MAXN*sizeof(int), cudaMemcpyHostToDevice);

  cudaStreamCreate(&stream0);
  cudaStreamCreate(&stream2);  
  
  cudaSetDevice(1);
  cudaMalloc((void**) &aa, MAXN*sizeof(int));
  cudaMalloc((void**) &ba, MAXN*sizeof(int));
  cudaMalloc((void**) &ca, MAXN*sizeof(int));

  cudaMemcpy(aa, A, MAXN*sizeof(int), cudaMemcpyHostToDevice);
  cudaMemcpy(ba, B, MAXN*sizeof(int), cudaMemcpyHostToDevice);
  cudaMemcpy(ca, C, MAXN*sizeof(int), cudaMemcpyHostToDevice);  

  cudaStreamCreate(&stream1);

  doAddone(0, a, &stream0);  
  doAddone(1, ba, &stream1);
  cudaDeviceSynchronize();  
  cudaError_t r=cudaGetLastError();
  printf("sync: %s\n", cudaGetErrorString(r));
  cudaMemcpyAsync(b, ba, MAXN*sizeof(int), cudaMemcpyDeviceToDevice, stream0);
  doAdd(0, a, b, c);

  cudaMemcpy(A, a, MAXN*sizeof(int), cudaMemcpyDeviceToHost);
  cudaMemcpy(B, ba, MAXN*sizeof(int), cudaMemcpyDeviceToHost);
  cudaMemcpy(C, c, MAXN*sizeof(int), cudaMemcpyDeviceToHost);

  for(int i = 0; i < MAXN; i++)
	printf("%d ", A[i]);
  puts("");
  
  for(int i = 0; i < MAXN; i++)
	printf("%d ", B[i]);
  puts("");
  
  for(int i = 0; i < MAXN; i++)
	printf("%d ", C[i]);
  puts("");

  cudaFree(a);
  cudaFree(b);
  cudaFree(c);  
  return 0;
}
