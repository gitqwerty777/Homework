#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
int main(int argc, char *argv[])
{
  assert(argc == 3);
  omp_set_num_threads(atoi(argv[1]));
  int n = atoi(argv[2]);
  printf("# of proc = %d\n", omp_get_num_procs());
  printf("# of loop iterations = %d\n", n);
#pragma omp parallel for
  for (int i = 0; i < n; i++) {
    printf("thread %d runs index %d.\n",
	   omp_get_thread_num(), i);
  }
  return 0;
}
