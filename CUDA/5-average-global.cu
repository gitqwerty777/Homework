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
  
  float sum = arr[((t+1)%2)*MAXN*MAXN+(i-1)*MAXN+j]
	+ arr[((t+1)%2)*MAXN*MAXN+(i-1)*MAXN+j+1]
	+ arr[((t+1)%2)*MAXN*MAXN+(i-1)*MAXN+j-1] 
	+ arr[((t+1)%2)*MAXN*MAXN+(i)*MAXN+j+1]
	+ arr[((t+1)%2)*MAXN*MAXN+(i)*MAXN+j-1]
	+ arr[((t+1)%2)*MAXN*MAXN+(i+1)*MAXN+j+1]
	+ arr[((t+1)%2)*MAXN*MAXN+(i+1)*MAXN+j]
	+ arr[((t+1)%2)*MAXN*MAXN+(i+1)*MAXN+j-1];

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
  dim3 grid=dim3(n,n,1);
  dim3 block=dim3(1,1,1);

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
