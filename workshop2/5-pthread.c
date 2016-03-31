#include <omp.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
/* main */

struct Input{
  int v;
  int i;
  int n;
};
typedef struct Input Input;

void* getInfo(void* p){
  struct Input* input = (Input*)p;
    input->v += input->i;
    printf("thread %d runs index %d with v = %d.\n",
	   omp_get_thread_num(), input->i, input->v);
    pthread_exit((int)p);
}

int main(int argc, char *argv[])
{
  assert(argc == 3);
  omp_set_num_threads(atoi(argv[1]));
  int n = atoi(argv[2]);
  printf("# of proc = %d\n", omp_get_num_procs());
  printf("# of loop iterations = %d\n", n);
  /* v */
  int v = 101;
  printf("before the loop thread %d with v = %d.\n",
	 omp_get_thread_num(), v);

  pthread_t thread[n];
  for(int i = 0; i < n; i++){
    Input* input = malloc(sizeof(Input));
    input->i = i;
    input->v = v;
    input->n = n;
    pthread_create(&thread[i], NULL, getInfo, (void*)(input));
  }
  //sync v
  Input* output;
  for(int i = 0; i < n-1; i++){
    pthread_join(thread[i], NULL);
  }
  pthread_join(thread[n-1], &output);
  v = output->v;
  //
  
  printf("after the loop thread %d with v = %d.\n",
	 omp_get_thread_num(), v);
  
  return 0;
}


