#include <cstdio>
int main(){
  int v = 63;
  for(int i = 0; i < 8; i++)
    fprintf(stderr, "%d", (v>>(7-i))%2);
}
