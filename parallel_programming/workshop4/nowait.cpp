#include <omp.h>
#include <cstdio>
#include <cassert>
#define MAXN 200000000

  int arr[MAXN];
int main(){
  //nowait  
  double start_t = omp_get_wtime();
#pragma omp parallel
  {
#pragma omp for nowait
    for(int i = 0; i < MAXN; i++){
      arr[i] = i;
    }
#pragma omp for    
    for(int i = 0; i < MAXN; i++){
      arr[i] += MAXN-i;
    }
  }
  double end_t = omp_get_wtime();

  for(int i = 0; i < MAXN; i++){  
    assert(arr[i]==MAXN);
  }
  fprintf(stderr, "nowait time = %lf\n", end_t - start_t);

  //no nowait
  start_t = omp_get_wtime();  
#pragma omp parallel
  {
#pragma omp for
    for(int i = 0; i < MAXN; i++){
    arr[i] = i;
  }
#pragma omp for    
    for(int i = 0; i < MAXN; i++){
      arr[i] += MAXN-i;
    }
  }
  end_t = omp_get_wtime();
  for(int i = 0; i < MAXN; i++){  
    assert(arr[i]==MAXN);
  }
  fprintf(stderr, "without nowait time = %lf\n", end_t - start_t);
  return 0;
}
