#include <cstdlib>
#include <cstdio>
#include <cuda_runtime_api.h>
#include <cuda.h>

//主函式
int main(){
  //網格和區塊大小設定
  dim3 grid=dim3(4,1,1);
  dim3 block=dim3(2,3,1);
  printf("gridDim  = dim3(%d,%d,%d)\n", grid.x,grid.y,grid.z);
  printf("blockDim = dim3(%d,%d,%d)\n", block.x,block.y,block.z);

  //計算總執行緒數
  int   num=grid.x*grid.y*grid.z*block.x*block.y*block.z;
  printf("total num of threads = %d\n", num);

  //配置主機記憶體 & 清空
  Index* h=(Index*)malloc(num*sizeof(Index));
  memset(h,0,num*sizeof(Index));

  //配置裝置記憶體 & 清空
  Index* d;
  cudaMalloc((void**) &d, num*sizeof(Index));
  cudaMemcpy(d, h, num*sizeof(Index), cudaMemcpyHostToDevice);

  //呼叫裝置核心.
  prob_idx_3d<<<grid,block>>>(d);

  //測試是否執行成功.
  cudaError_t r=cudaGetLastError();
  printf("prob_idx_3d: %s\n", cudaGetErrorString(r));
  if(r!=0) goto end;

  //下載裝置記憶體內容到主機上.
  cudaMemcpy(h, d, num*sizeof(Index), cudaMemcpyDeviceToHost);

  //顯示內容
  for(int i=0; i<num; i++){
	printf("h[%d]={block:(%d,%d,%d), thread:(%d,%d,%d)}\n", i,
		   h[i].block.x,  h[i].block.y,  h[i].block.z,
		   h[i].thread.x, h[i].thread.y, h[i].thread.z
		   );
  }

 end:;
  //釋放裝置記憶體.
  cudaFree(d);
  free(h);

  return 0;
}
