#include <cstdio>
#include <omp.h>
#include <vector>
#include <cstdlib>
#include <unistd.h>
#define MAX_TASK_NUM 15

int p, c, n;
int workPool[MAX_TASK_NUM];
int head = 0;
int size = 0;

void producer(int num){
  while(1){
    if(size < MAX_TASK_NUM && n > 0){
      int sleepTime = rand()%10+1;
#pragma omp critical
      {
	if(size < MAX_TASK_NUM && n > 0){
	  fprintf(stderr, "producer %d: add task %d\n", num, sleepTime);	  	  	    
	  n--;
	  workPool[(head+size)%MAX_TASK_NUM] = sleepTime;	  	  
	  size++;
	  fprintf(stderr, "pool: ");
	  for(int j = 0; j < size; j++){
	    fprintf(stderr, "%d ", workPool[(head+j)%MAX_TASK_NUM]);
	  }
	  fprintf(stderr, "\n\n");
	}
      }
    } else {
      sleep(1);
    }
  }
}

void consumer(int num){
  while(1){
    if(size > 5){
      int i;
      bool isSuccess = true;
#pragma omp critical
      {
	if(size > 5){
	  i = workPool[head];
	  fprintf(stderr, "consumer %d: sleep %d\n",num,  i);	    	    	    
	  head++;
	  size--;
	  fprintf(stderr, "pool: ");
	  for(int j = 0; j < size; j++){
	    fprintf(stderr, "%d ", workPool[(head+j)%MAX_TASK_NUM]);
	  }
	  fprintf(stderr, "\n\n");
	} else {
	  isSuccess = false;
	}
      }
      if(isSuccess)
	sleep(float(i)/3);
    } else {
      sleep(1);
    }
  }
}

int main(int argc, char *argv[]){

  
  p = atoi(argv[1]);
  c = atoi(argv[2]);
  n = atoi(argv[3]);

  omp_set_num_threads(5);
#pragma omp parallel sections
  {
#pragma omp section
    {
      producer(1);
    }
#pragma omp section
    {
      producer(2);
    }
#pragma omp section
    {
      consumer(1);
    }
#pragma omp section
    {
      consumer(2);
    }
#pragma omp section
    {
      consumer(3);
    }    
  }
}
