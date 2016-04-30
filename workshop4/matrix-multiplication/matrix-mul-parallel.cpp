#include <cstdio>
#include <omp.h>
#include <cassert>
#include <stdint.h>
#include <cstdlib>
#include <climits>

// generate matrix, row-major
#define UINT uint32_t
UINT* rand_gen(UINT seed, int R, int C) {
  UINT* m;
    m = (UINT *) malloc(sizeof(UINT) * R*C);
  UINT x = 2, n = R*C;
  for (int i = 0; i < R; i++) {
    for (int j = 0; j < C; j++) {
      x = (x * x + seed + i + j)%n;
      m[i*C + j] = x;
    }
  }
  return m;

}
inline UINT hash(UINT x) {
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
long long minCost[101][101];
int cutpoint[101][101];
int matrixSize[105];
int matrixSeed[105];

int min(int a, int b){
  return (a<b)?a:b;
}

inline int connectCost(int start, int mid, int end){//only used in begin and mid, end in the cut point
  //  if(start>=mid || mid>=end || start>=end)
  //return 0;
  return matrixSize[start] * matrixSize[mid+1] * matrixSize[end+1];
}
inline int connectCost(int start, int end){//only used in begin and mid, end in the cut point
  return matrixSize[start] * matrixSize[end] * matrixSize[end+1];
}
inline int connect3Cost(int start, int end){//only used in begin and mid, end in the cut point
  int c1 =matrixSize[start] * matrixSize[start+1] * matrixSize[start+2] + matrixSize[start] * matrixSize[start+2] * matrixSize[start+3];
  //fprintf(stderr, "cut point = %d, get cost = %d\n", -1, c1);        
  int c2 = matrixSize[start+1] * matrixSize[start+2] * matrixSize[start+3] + matrixSize[start] * matrixSize[start+1] * matrixSize[start+3];
  //fprintf(stderr, "cut point = %d, get cost = %d\n", start, c2);            
  if(c1 <= c2){
    return c1;
  } else {
    cutpoint[start][end] = start;//not sure
    return c2;
  }
}

long long getCost(int start, int end){
  for(int i = 1; i < N; i++){//interval
    for(int j = 0; j+i < N; j++){//want cutpoint[j][j+i]
      #pragma omp parallel for
      for(int k = j+1; k < j+i; k++){//iterate all mincost possibilities
	int newcost = minCost[j][k]+minCost[k+1][j+i]+connectCost(j, k, j+i);
	if (minCost[j][j+i] > newcost){
#pragma omp critical
	  {
	    if (minCost[j][j+i] > newcost){
	      minCost[j][j+i] = newcost;
	      cutpoint[j][j+i] = k;
	    }
	  }
	}
      }
    }
  }
  return minCost[start][end];
}

inline UINT* multiply(UINT* A, UINT* B, UINT* C, int a, int b, int c){//a, b, c is size of matrixA, B
  C = (UINT*)malloc(sizeof(UINT)*a*c);
  //fprintf(stderr, "for size %d, %d, %d\n", a, b, c);
#pragma omp parallel for collapse(2)
  for (int i = 0; i < a; i++)
    for (int j = 0; j < c; j++){
      int ci = i*c+j;
      C[ci] = 0;
      for (int k = 0; k < b; k++){
	C[ci] += A[i*b+k] * B[k*c+j];
      }
    }
  return C;
}

UINT* calculateSequenceMatrixs(int start, int end){
  //cprintf("calculate sequence %d~%d\n", start, end);
  UINT* startMatrix = rand_gen(matrixSeed[start], matrixSize[start], matrixSize[start+1]);
  /*for(int i = 0; i < matrixSize[start]; i++){
    for(int j = 0; j < matrixSize[start+1]; j++){
      printf("%zu ", startMatrix[i*matrixSize[start]+j]);
    }
    printf("\n");
    }*/
  for(int i = start+1; i <= end; i++){
    UINT* middleMatrix = rand_gen(matrixSeed[i], matrixSize[i], matrixSize[i+1]);
    /*for(int t = 0; t < matrixSize[start]; t++){
      for(int j = 0; j < matrixSize[end+1]; j++){
	printf("%zu ", middleMatrix[t*matrixSize[start]+j]);
      }
      printf("\n");
      }*/
    UINT* outputMatrix;
    outputMatrix = multiply(startMatrix, middleMatrix, outputMatrix, matrixSize[start], matrixSize[i], matrixSize[i+1]);
    /*for(int t = 0; t < matrixSize[start]; t++){
      for(int j = 0; j < matrixSize[i+1]; j++){
	printf("%zu ", outputMatrix[t*matrixSize[start]+j]);
      }
      printf("\n");
      }*/
    free(startMatrix);
    startMatrix = outputMatrix;
  }
  return startMatrix;
}

UINT* calculateMatrixs(int start, int end){
  //find cut point, and start, mid, end
  int mid = cutpoint[start][end];
  UINT* outputm;
  if(mid == -1){//no cut point, calcuate as normal
    outputm = calculateSequenceMatrixs(start, end);
  } else {
    UINT *m1, *m2;
    m1 = calculateMatrixs(start, mid);
    m2 = calculateMatrixs(mid+1, end);
    outputm = multiply(m1, m2, outputm, matrixSize[start], matrixSize[mid+1], matrixSize[end+1]);
  }
  return outputm;
}

int main(){
  #ifdef OPENMP
  omp_set_num_threads(20);
  #endif
  while(scanf("%d", &N) == 1){
    for(int i = 0; i < N; i++)
      for(int j = 0; j < N; j++)
	cutpoint[i][j] = -1;
    for(int i = 0; i < N; i++)
      for(int j = 0; j < N; j++)
	minCost[i][j] = 0;    
    for(int i = 0; i < N+1; i++)
      scanf("%d", &matrixSize[i]);
    for(int i = 0; i < N; i++)
      scanf("%d", &matrixSeed[i]);

    int start = 0, end = N-1;
    long long cost = getCost(start, end);
    fprintf(stderr, "min cost: %lld\n", cost);
    fprintf(stderr, "cut point = %d\n", cutpoint[start][end]);
  
    //start calculating
    UINT* ans;
    ans = calculateMatrixs(start, end);

    /*for(int i = 0; i < matrixSize[start]; i++){
      for(int j = 0; j < matrixSize[end+1]; j++){
	printf("%zu ", ans[i*matrixSize[start]+j]);
      }
      printf("\n");
      }*/
    printf("%u\n", signature(ans, matrixSize[start], matrixSize[end+1]));
  }
return 0;
}
 
