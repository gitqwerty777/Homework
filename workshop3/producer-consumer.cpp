#include <cstdio>
#include <omp.h>
#include <vector>
#include <cstdlib>
#include <unistd.h>

int main(int argc, char *argv[]){
  std::vector<int> workPool;
  workPool.clear();
  
  int p = atoi(argv[1]);
  int c = atoi(argv[2]);
  int n = atoi(argv[3]);

  omp_set_num_threads(2);
#pragma omp parallel sections
  {
#pragma omp section
    {
      fprintf(stderr, "producer\n");
      while(1){
	if(workPool.size() < 15 && n > 0){
#pragma omp critical
	  n--;	
	  int sleepTime = rand()%10+1;
#pragma omp critical
	  workPool.push_back(sleepTime);
	  fprintf(stderr, "add task %d\n", sleepTime);
	} else {
	  sleep(1);
	}
      }
    }
#pragma omp section
    {
      fprintf(stderr, "consumer\n");
      while(1){
	if(workPool.size() > 5){
	  int i;
#pragma omp critical
	  {
	    i = workPool[0];
	    workPool.erase(workPool.begin());
	  }
	  fprintf(stderr, "sleep %d\n", i);
	  sleep(float(i)/10);
	} else {
	  sleep(1);
	}
      }
    }
  }
}
