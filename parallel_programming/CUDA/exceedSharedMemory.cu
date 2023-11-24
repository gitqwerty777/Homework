//索引用到的緒構體
struct Index{
  uint3 block, thread;
};

//核心:把索引寫入裝置記憶體
__global__ void prob_idx_3d(Index* id){

  __shared__ int test[100000];
  
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
