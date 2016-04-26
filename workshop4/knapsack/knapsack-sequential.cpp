#include <cstdio>
#include <omp.h>
#include <cmath>

int N, W;
int item[10000][2];
int dp[2][10000000];

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
      int a = getMaxValue(1, item[0][0], item[0][1]);
      int b = getMaxValue(1, 0, 0);
      printf("%d", max(a, b));
  return 0;
}
