#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <cuda.h>
#define MAXGPU 10
#define BSIDE 16
#define MAXK 40960
#define MAXN 2048

int m, n;
float arr[2*MAXN*MAXN];

__global__ void goNextState(int n, int t, float *arr){
  //global and local
  int i = blockDim.x*blockIdx.x+threadIdx.x+1;
  int j = blockDim.y*blockIdx.y+threadIdx.y+1;
  int li = threadIdx.x+1;
  int lj = threadIdx.y+1;
  
  __shared__ float sur[BSIDE+2][BSIDE+2];//surroundings

  float original = arr[((t+1)%2)*MAXN*MAXN+(i)*MAXN+j];
  sur[li][lj] = original;
  //debugInfo[i*MAXN+j].globalValue = original;

  if(li == 1){
	sur[0][lj] = arr[((t+1)%2)*MAXN*MAXN+(i-1)*MAXN+j];
	if(lj == 1){//corner
	  sur[0][0] = arr[((t+1)%2)*MAXN*MAXN+(i-1)*MAXN+j-1];
	} else if(lj == BSIDE || j == n){
	  sur[0][lj+1] = arr[((t+1)%2)*MAXN*MAXN+(i-1)*MAXN+j+1];
	}
  }
  if(lj == 1){
	sur[li][0] = arr[((t+1)%2)*MAXN*MAXN+i*MAXN+j-1];
  }
  if(lj == BSIDE || j == n){
	sur[li][lj+1] = arr[((t+1)%2)*MAXN*MAXN+i*MAXN+j+1];
  }    
  if(li == BSIDE || i == n){
	sur[li+1][lj] = arr[((t+1)%2)*MAXN*MAXN+(i+1)*MAXN+j];      
	if(lj == 1){
	  sur[li+1][0] = arr[((t+1)%2)*MAXN*MAXN+(i+1)*MAXN+j-1];
	} else if(lj == BSIDE || j == n){
	  sur[li+1][lj+1] = arr[((t+1)%2)*MAXN*MAXN+(i+1)*MAXN+j+1];
	}
  }

  __syncthreads();

  if(!(i <= n && j <= n)) return;  
  
  //printf("global (%d, %d), local (%d, %d)\n", i, j, li, lj);
  float sum = 0;          
  //use add
  sum += sur[li-1][lj] + 
	+ sur[li-1][lj+1]
	+ sur[li-1][lj-1]
	+ sur[li][lj+1]
	+ sur[li][lj-1]
	+ sur[li+1][lj+1]
	+ sur[li+1][lj]
	+ sur[li+1][lj-1];
  arr[((t)%2)*MAXN*MAXN+(i)*MAXN+j] = sum/8;
}

int main() {
  scanf("%d %d", &n, &m);
  for(int i = 1; i <= n; i++)
    for(int j = 1;j <= n; j++)
      arr[i*MAXN+j] = (i-1)*MAXN+(j-1);
  
  //網格和區塊大小設定
  int globalN = n;
  while(globalN % BSIDE)
	globalN++;
  int gridNum = globalN/BSIDE;
  dim3 grid=dim3(gridNum,gridNum,1);
  dim3 block=dim3(BSIDE,BSIDE,1);

  //配置裝置記憶體
  float* arr_device;
  struct DebugInfo *debuginfo_device;
  cudaMalloc((void**) &arr_device, 2*MAXN*MAXN*sizeof(float));
  cudaMemcpy(arr_device, arr, 2*MAXN*MAXN*sizeof(float), cudaMemcpyHostToDevice);
#ifdef DEBUG
  cudaMalloc((void**) &debuginfo_device, 2*MAXN*MAXN*sizeof(struct DebugInfo));
  cudaMemcpy(debuginfo_device, debugInfo, 2*MAXN*MAXN*sizeof(struct DebugInfo), cudaMemcpyHostToDevice);
#endif

  //呼叫裝置核心.
  for(int t = 1; t <= m; t++){
	goNextState<<<grid,block>>>(n, t, arr_device);
	//測試是否執行成功.	
	cudaError_t r=cudaGetLastError();
	//printf("gonextstate: %s\n", cudaGetErrorString(r));
	if(r!=0) goto end;
  }

  //下載裝置記憶體內容到主機上.
  cudaMemcpy(arr, arr_device, 2*MAXN*MAXN*sizeof(float), cudaMemcpyDeviceToHost);
  //下載裝置記憶體內容到主機上.
#ifdef DEBUG  
  cudaMemcpy(debugInfo, debuginfo_device, 2*MAXN*MAXN*sizeof(struct DebugInfo), cudaMemcpyDeviceToHost);
#endif
  
  /*  for(int i = 1; i <= n; i++){
	  for(int j = 1; j <= n; j++){
	  printf("[%d][%d]: globali = %d, globalj = %d, blockidx = %d, blockidy = %d, threadidx = %d, threadidy = %d, globalvalue = %d, count = %d\n", i, j, debugInfo[i*MAXN+j].i, debugInfo[i*MAXN+j].j, debugInfo[i*MAXN+j].blockidx, debugInfo[i*MAXN+j].blockidy, debugInfo[i*MAXN+j].threadidx, debugInfo[i*MAXN+j].threadidy, debugInfo[i*MAXN+j].globalValue, debugInfo[i*MAXN+j].count);
	  }
	  }*/
  
  //顯示內容
  for(int i = 1; i <= n; i++){
    for(int j = 1; j <= n; j++){
      printf("%f ", arr[((m)%2)*MAXN*MAXN+i*MAXN+j]);
    }
    puts("");
  }  

 end:;
  //釋放裝置記憶體.
  cudaFree(arr_device);
#ifdef DEBUG
  cudaFree(debuginfo_device);
#endif
  return 0;

}
