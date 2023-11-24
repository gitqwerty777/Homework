#include <stdio.h>
#include <stdint.h>
// #define DEBUG
#define UINT uint32_t
#define MAXN 2048
void multiply(int N, UINT A[][2048], UINT B[][2048], UINT C[][2048]) {
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      UINT sum = 0;    // overflow, let it go.
      for (int k = 0; k < N; k++)
	sum += A[i][k] * B[k][j];
      C[i][j] = sum;
    }
  }
}
void rand_gen(UINT c, int N, UINT A[][MAXN]) {
  UINT x = 2, n = N*N;
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      x = (x * x + c + i + j)%n;
      A[i][j] = x;
    }
  }
}
void print_matrix(int N, UINT A[][MAXN]) {
  for (int i = 0; i < N; i++) {
    fprintf(stderr, "[");
    for (int j = 0; j < N; j++)
      fprintf(stderr, " %u", A[i][j]);
    fprintf(stderr, " ]\n");
  }
}
UINT signature(int N, UINT A[][MAXN]) {
  UINT h = 0;
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++)
      h = (h + A[i][j]) * 2654435761LU;
  }
  return h;
}
UINT A[MAXN][MAXN], B[MAXN][MAXN], C[MAXN][MAXN];
int main() {
  int N;
  uint32_t S1, S2;
  scanf("%d %u %u", &N, &S1, &S2);
  rand_gen(S1, N, A);
  rand_gen(S2, N, B);
  multiply(N, A, B, C);
#ifdef DEBUG
  print_matrix(N, A);
  print_matrix(N, B);
  print_matrix(N, C);
#endif
  printf("%u\n", signature(N, C));
  return 0;
}
