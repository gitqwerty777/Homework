#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main(){
  omp_set_num_threads(4);
  #pragma omp parallel for
  for(int i = 0; i < 4; i++){
    srand(i);
    for(int j = 0; j < 10; j++){
      fprintf(stderr, "thread %d: %d\n",i, rand());
    }
  }
}
