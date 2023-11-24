#include <cstdlib>
#include <cstdio>
#include <cuda_runtime_api.h>
#include <cuda.h>

//索引用到的緒構體
struct Index{
  uint3 block, thread;
};

//核心:把索引寫入裝置記憶體
__global__ void prob_idx_3d(Index* id){

  //計算區塊索引
  int b=(blockIdx.z*gridDim.y+blockIdx.y)*gridDim.x+blockIdx.x;
  //計算執行緒索引
  int t=(threadIdx.z*blockDim.y+threadIdx.y)*blockDim.x+threadIdx.x;
  //計算區塊中包含的執行緒數目
  int n=blockDim.x*blockDim.y*blockDim.z;
  //執行緒在陣列中對應的位置
  int x=b*n+t;

  //每個執行緒寫入自己的區塊和執行緒索引.
  id[x].block=blockIdx;
  id[x].thread=threadIdx;
}

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
