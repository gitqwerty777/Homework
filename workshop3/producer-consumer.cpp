#include <cstdio>
#include <omp.h>
#include <vector>
#include <cstdlib>
#include <unistd.h>
#define MAX_TASK_NUM 15

int main(int argc, char *argv[]){
  int workPool[MAX_TASK_NUM];
  int head = 0;
  int size = 0;
  
  int p = atoi(argv[1]);
  int c = atoi(argv[2]);
  int n = atoi(argv[3]);

  omp_set_num_threads(2);
#pragma omp parallel sections
  {
#pragma omp section
    {
      while(1){
	if(size < MAX_TASK_NUM && n > 0){
	  int sleepTime = rand()%10+1;
#pragma omp critical
	  {
	  fprintf(stderr, "add task %d\n", sleepTime);	  	  	    
	  n--;
	  workPool[(head+size)%MAX_TASK_NUM] = sleepTime;	  	  
	  size++;
	  for(int j = 0; j < size; j++){
	    fprintf(stderr, "%d ", workPool[(head+j)%MAX_TASK_NUM]);
	  }
	  fprintf(stderr, "\n");
	  }
	} else {
	  sleep(1);
	}
      }
    }
#pragma omp section
    {

      while(1){
	if(size > 5){
	  int i;
#pragma omp critical
	  {
	    i = workPool[head];
	  fprintf(stderr, "sleep %d\n", i);	    	    	    
	    head++;
	    size--;
	    for(int j = 0; j < size; j++){
	      fprintf(stderr, "%d ", workPool[(head+j)%MAX_TASK_NUM]);
	    }
	    fprintf(stderr, "\n");
	  }
	  sleep(float(i)/3);
	} else {
	  sleep(1);
	}
      }
    }
  }
}
