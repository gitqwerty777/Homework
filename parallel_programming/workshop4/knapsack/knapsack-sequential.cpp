#include <cstdio>
#include <omp.h>
#include <cmath>
#include <cstring>
#define MAXW 1000001


int N, W;
int item[10000][2];
int dp[2][MAXW];//double buffer

inline int max(int a, int b){
  return (a > b)?a:b;
}

int getMaxValue(int i, int w, int v){
  if(i == N)
    if(w <= W)
      return v;
    else
      return 0;
  int a = getMaxValue(i+1, w+item[i][0], v+item[i][1]);
  int b = getMaxValue(i+1, w, v);
  return max(a, b);
}

int main(){
  scanf("%d %d", &N, &W);
  for(int i =0; i < N; i++){
    scanf("%d %d", &item[i][0], &item[i][1]);//weight and value
  }

  //initial: dp when t = 0
#pragma omp parallel for
  for(int i = 0; i <= W; i++){
  dp[0][i] = -1;
  dp[1][i] = -1;
	}
  dp[0][0] = 0;
  dp[1][0] = 0;
  dp[0][item[0][0]] = item[0][1];
  dp[1][item[0][0]] = item[0][1];  

  //t=1
#pragma omp parallel
  {
    for(int t = 1; t < N; t++){
      int nowt = t%2;
      int pret = (nowt==0)?1:0;
      #pragma omp for
      for(int i = 0; i <= W; i++){
  if(dp[pret][i] != -1 && (i+item[t][0] <= W)){
	  dp[nowt][i+item[t][0]] = max(dp[nowt][i+item[t][0]], dp[pret][i]+item[t][1]);
	  //fprintf(stderr, "add dp item %d, w = %d, v = %d\n", t, i+item[nowt][0], dp[nowt][i+item[nowt][0]]);
	}
      }
#pragma omp for
      for(int i = 0; i <= W; i++){
	dp[pret][i] = dp[nowt][i];
	//printf("now t = %d, w = %d, v = %d\n", t, i, dp[pret][i]);
      }
    }
  }
  int maxv = 0;
  for(int i = 0; i <= W; i++){
    if(maxv < dp[(N-1)%2][i])
      maxv = dp[(N-1)%2][i];
  }
  printf("%d", maxv);
  return 0;
}
