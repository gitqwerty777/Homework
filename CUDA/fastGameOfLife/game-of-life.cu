#define CL_USE_DEPRECATED_OPENCL_2_0_APIS 

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <CL/cl.h>
#define MAXGPU 10
#define BSIDE 8
#define MAXK 40960
#define MAXN 2048

int m, n;
unsigned int arr[2*MAXN*MAXN];

#define MAXN 2048
#define BSIDE 8

__global__ void goNextState(int n, int t, unsigned int *arr){
  //計算區塊索引
  int b=(blockIdx.z*gridDim.y+blockIdx.y)*gridDim.x+blockIdx.x;
  //計算執行緒索引
  int threadnum=(threadIdx.z*blockDim.y+threadIdx.y)*blockDim.x+threadIdx.x;
  //計算區塊中包含的執行緒數目
  int blockthread=blockDim.x*blockDim.y*blockDim.z;
  //執行緒在陣列中對應的位置
  int x=b*n+t;
  //printf("b %d t %d n %d x %d\n", b, threadnum, blockthread, x);
  /*int i = get_global_id(0)+1;
  int j = get_global_id(1)+1;
  int li = get_local_id(0)+1;
  int lj = get_local_id(1)+1;
  
  __shared__ unsigned int sur[BSIDE+2][BSIDE+2];//surroundings
  
  int original = arr[((t+1)%2)*MAXN*MAXN+(i)*MAXN+j];
  sur[li][lj] = original;

  if(!(i <= n && j <= n)) return;  
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

  
  barrier(CLK_LOCAL_MEM_FENCE);

  if(!(i <= n && j <= n)) return;

  //printf("global (%d, %d), local (%d, %d)\n", i, j, li, lj);
  int count = 0;          
  //use add
  if(sur[li-1][lj] == '1')
    count++;
  if(sur[li-1][lj+1] == '1')
    count++;
  if(sur[li-1][lj-1] == '1')
    count++;    
  if(sur[li][lj+1] == '1')
    count++;
  if(sur[li][lj-1] == '1')
    count++;
  if(sur[li+1][lj+1] == '1')
    count++;
  if(sur[li+1][lj] == '1')
    count++;    
  if(sur[li+1][lj-1] == '1')
    count++;

  if((original == '1') && !((count == 2) || (count == 3))){
    arr[((t)%2)*MAXN*MAXN+i*MAXN+j] = '0';
    //counts[((t+1)%2)*MAXN*MAXN+i*MAXN+j] = 0;
  } else if((original == '0') && (count == 3)){
    arr[((t)%2)*MAXN*MAXN+i*MAXN+j] = '1';    
    //counts[((t+1)%2)*MAXN*MAXN+i*MAXN+j] = 1;
  } else{
    arr[((t)%2)*MAXN*MAXN+i*MAXN+j] = original;
  }
  */
}

int main() {
  char s[2004];
  scanf("%d %d\n", &n, &m);
  for(int i = 1; i <= n; i++){
    scanf("%s", s);
    for(int j = 1;j <= n; j++)
      arr[i*MAXN+j] = s[j-1];
  }
  
  //網格和區塊大小設定
  dim3 grid=dim3(2,1,1);
  dim3 block=dim3(1,1,1);

  //計算總執行緒數
  int num=grid.x*grid.y*grid.z*block.x*block.y*block.z;

  //配置裝置記憶體
  unsigned int* arr_device;
  cudaMalloc((void**) &arr_device, 2*MAXN*MAXN*sizeof(unsigned int));
  cudaMemcpy(arr_device, arr, 2*MAXN*MAXN*sizeof(unsigned int), cudaMemcpyHostToDevice);

  //呼叫裝置核心.
  for(int t = 0; t < m; t++)
	goNextState<<<grid,block>>>(n, t, arr_device);

  //測試是否執行成功.
  cudaError_t r=cudaGetLastError();
  printf("gonextstate: %s\n", cudaGetErrorString(r));
  if(r!=0) goto end;

  //下載裝置記憶體內容到主機上.
  cudaMemcpy(arr, arr_device, 2*MAXN*MAXN*sizeof(unsigned int), cudaMemcpyDeviceToHost);
  
  //顯示內容
  for(int i = 1; i <= n; i++){
    for(int j = 1; j <= n; j++){
      putchar((arr[((m)%2)*MAXN*MAXN+i*MAXN+j]==0)?'0':'1');
    }
    puts("");
  }  

 end:;
  //釋放裝置記憶體.
  cudaFree(arr_device);
  return 0;

}
