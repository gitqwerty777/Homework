#include "utils.h"
 
int ret[128];
int run(int n, int key) {
  int sum = 0;
  f(n, key, ret, ret+16, ret+32, ret+48);
  for (int i = 0; i < 4; i++)
    sum += ret[i*16];
  return sum;
}
