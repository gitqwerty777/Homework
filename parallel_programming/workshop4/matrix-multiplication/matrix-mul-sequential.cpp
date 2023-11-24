#include <cstdio>
#include <omp.h>
#include <cassert>
#include <stdint.h>
#include <cstdlib>
#define INF 2147483647

// generate matrix, row-major
#define UINT uint32_t
UINT* rand_gen(UINT seed, int R, int C) {
  UINT *m = (UINT *) malloc(sizeof(UINT) * R*C);
  UINT x = 2, n = R*C;
  for (int i = 0; i < R; i++) {
    for (int j = 0; j < C; j++) {
      x = (x * x + seed + i + j)%n;
      m[i*C + j] = x;
    }
  }
  return m;
}
UINT hash(UINT x) {
  return (x * 2654435761LU);
}
// output
UINT signature(UINT *A, int r, int c) {
  UINT h = 0;
  for (int i = 0; i < r; i++) {
    for (int j = 0; j < c; j++)
      h = hash(h + A[i*c + j]);
  }
  return h;
}

int N;
int dp[101][101];
int cutpoint[101][101];
int matrixSize[105];
int matrixSeed[105];

int min(int a, int b){
  (a<b)?a:b;
}

int connectCost(int start, int end){//only used in begin and end in the cut point
  return matrixSize[start] * matrixSize[end] * matrixSize[end+1];
}

int getCost(int start, int end){
  assert(start != end);
  if(start+1 == end)
    return connectCost(start, end);
  int nowcost = INF;
  for(int i = start; i <= end; i++)
    nowcost = min(nowcost, getCost(start, i) + getCost(i, end) + connectCost(start, end));
  return nowcost;
}

void multiply(UINT* A, UINT* B, UINT* C, int a, int b, int c){
/*#pragma omp parallel for  
  for (int i = 0; i < b; i++) {
    for (int j = 0; j < a; j++) {
      unsigned long sum = 0;    // overflow, let it go.
      for (int k = 0; k < c; k++)
	sum += A[i][k] * B[k][j];
      C[i][j] = sum;
    }
    }*/
}


UINT* calculateSequenceMatrixs(int start, int end){
  UINT* startMatrix = rand_gen(matrixSeed[start], matrixSize[start], matrixSize[start+1]);      
  for(int i = start+1; i < end; i++){
    UINT* middleMatrix = rand_gen(matrixSeed[i], matrixSize[i+1], matrixSize[i+1]);
    UINT* outputMatrix;//malloc
//multiply(startMatrix, middleMatrix, outputMatrix);
    startMatrix = outputMatrix;
  }
  return startMatrix;
}

UINT* calculateMatrixs(int start, int end){
  //if too small to divide, consequence
  calculateSequenceMatrixs(start, end);
  //find cut point, and start, mid, end
int mid = cutpoint[start][end];
  cutpoint[start];
calculateMatrixs(start, mid);
  calculateMatrixs(mid, end);
  UINT* startMatrix = rand_gen(matrixSeed[start], matrixSize[start], matrixSize[start+1]);
  UINT* endMatrix = rand_gen(matrixSeed[start], matrixSize[start], matrixSize[start+1]);
}




int main(){
  scanf("%d", &N);
  for(int i = 0; i < N+1; i++){
    scanf("%d", &matrixSize[i]);
  }
  for(int i = 0; i < N; i++){
    scanf("%d", &matrixSeed[i]);
  }

  int start = 0, end = N-1;
  int cost = getCost(start, end);
  printf("min cost: %d\n", dp[start][end]);
  printf("cut point = %d\n", cutpoint[start][end]);
  
//start calculating
UINT* ans = calculateMatrixs(start, end);
//return signature(ans, r, c);
return 0;
}

