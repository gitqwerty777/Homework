#define MAXN 1024
#define BSIDE 16

__kernel void multiply(int N, int globalN, __global unsigned int A[MAXN][MAXN], __global unsigned int B[MAXN][MAXN], __global unsigned int C[MAXN][MAXN]){
  int i = get_global_id(0);
  int j = get_global_id(1);
  int li = get_local_id(0);
  int lj = get_local_id(1);
  
  int BLK = globalN/BSIDE;
  
  __local unsigned int ALocal[BSIDE][BSIDE];
  __local unsigned int BLocal[BSIDE][BSIDE];

  unsigned int sum = 0;
  for (int block = 0; block < BLK; block++) {
	//if(i < N && j < N){
	if(block*BSIDE+lj < N)
	  ALocal[li][lj] = A[i][block * BSIDE + lj];
	if(block*BSIDE+li < N)
	  BLocal[li][lj] = B[block * BSIDE + li][j];
	  //} else{
	  //	  ALocal[li][lj] = 0;
	  //	  BLocal[li][lj] = 0;
	  //	}
    barrier(CLK_LOCAL_MEM_FENCE);
	for (int k = 0; k < BSIDE; k++) {
	  sum += ALocal[li][k] * BLocal[k][lj];
	}
    barrier(CLK_LOCAL_MEM_FENCE);
  }
  C[i][j] = sum;
}

__kernel void add(int N, __global unsigned int A[MAXN][MAXN], __global unsigned int B[MAXN][MAXN], __global unsigned int C[MAXN][MAXN]){
  int i = get_global_id(0);
  for (int j = 0; j < N; j++)
	C[i][j] = A[i][j] + B[i][j];
}

