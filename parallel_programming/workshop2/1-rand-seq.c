#include <stdio.h>
#include <stdlib.h>

int main(){
  srand(0);
  for(int i = 0; i < 10; i++){
    fprintf(stderr, "%d\n", rand());
  }
}
