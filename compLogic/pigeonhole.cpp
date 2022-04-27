#include <cstdio>
#include <cstdlib>

int main(int argc, char** argv){
  int m, n;
  m = atoi(argv[1]);
  n = atoi(argv[2]);

  printf("c pigeonhole: %d poles, %d pigeons\n", m, n);
  printf("p cnf %d %d\n", m*n, n + (n)*(n-1)/2*m);

  //p_ij = i * m + j

  //pigeon i lives in a hole
  for(int i = 0; i < n; i++){
    for(int j = 0; j < m; j++){
      printf("%d ", i*m+j+1);
    }
    puts("0");
  }

  //at most one pigeon lives in hole k
  for(int k = 0; k < m; k++)
    for(int i = 0; i < n; i++)
      for(int j = i+1; j < n; j++)
        printf("%d %d 0\n", -(i*m+k+1), -(j*m+k+1));//p_ik, p_jk

  return 0;
}
