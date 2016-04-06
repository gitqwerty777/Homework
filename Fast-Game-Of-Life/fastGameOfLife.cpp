#include <cstdio>
#include <omp.h>
 
int arr[2002][2002] = {{0}};
int newarr[2002][2002] = {{0}};
 
int main(){
  int m, n;
  char s[2002];
  scanf("%d %d", &n, &m);
  for(int i = 1; i <= n; i++){
    scanf("%s", s);
    for(int j = 1;j <= n; j++)
      arr[i][j] = s[j-1]-'0';
  }
  /*fprintf(stderr, "before iteration\n");
  for(int r = 1; r <= n; r++){
    for(int c = 1; c <= n; c++){
          printf("%d ", arr[r][c]);    
    }
    printf("\n");          
    }*/
 
  for(int t = 1; t <= m; t++){
    //fprintf(stderr, "turn %d\n", t);
#pragma omp parallel for collapse(2)
    for(int i = 1; i <= n; i++){
      for(int j = 1; j <= n; j++){
	int count = 0;
	if(arr[i-1][j])
	  count++;
	if(arr[i+1][j])
	  count++;
	if(arr[i][j-1])
	  count++;
	if(arr[i][j+1])
	  count++;
	if(arr[i+1][j+1])
	  count++;
	if(arr[i-1][j+1])
	  count++;
	if(arr[i+1][j-1])
	  count++;
	if(arr[i-1][j-1])
	  count++;
 
	if((arr[i][j] == 1) && !(count == 2 || count == 3)){
	  newarr[i][j] = 0;
	} else if((arr[i][j] == 0) && (count == 3)){
	  newarr[i][j] = 1;
	} else {
	  newarr[i][j] = arr[i][j];
	}
	//fprintf(stderr, "thread %d - %d, %d, arr = %d->%d, count = %d\n", omp_get_thread_num(), i, j, arr[i][j], newarr[i][j], count);
      }
    }
 
    //fprintf(stderr, "after %d iteration\n", t);
    for(int r = 1; r <= n; r++){
      for(int c = 1; c <= n; c++){
	//printf("%d ", newarr[r][c]);    
	arr[r][c] = newarr[r][c];
      }
      //printf("\n");          
    }
  } 
  for(int i = 1; i <= n; i++){
    for(int j = 1; j <= n; j++)
      printf("%d", arr[i][j]);
    printf("\n");    
  }
  return 0;
}
