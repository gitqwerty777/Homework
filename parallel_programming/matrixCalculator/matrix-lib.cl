#define MAXN 1024

__kernel void multiply(int N, __global unsigned int A[MAXN][MAXN], __global unsigned int B[MAXN][MAXN], __global unsigned int C[MAXN][MAXN]){
  int i = get_global_id(0);
  //int j = get_global_id(1)+1;
  //int li = get_local_id(0)+1;
  //int lj = get_local_id(1)+1;
  for (int j = 0; j < N; j++) {
	unsigned int sum = 0;    // overflow, let it go.
	for (int k = 0; k < N; k++)
	  sum += A[i][k] * B[k][j];
	C[i][j] = sum;
  }
}

__kernel void add(int N, __global unsigned int A[MAXN][MAXN], __global unsigned int B[MAXN][MAXN], __global unsigned int C[MAXN][MAXN]){
  int i = get_global_id(0);
  for (int j = 0; j < N; j++)
	C[i][j] = A[i][j] + B[i][j];
}
