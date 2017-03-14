#include <cstdio>
#include <cstring>
#include <cassert>
#include <omp.h>

int arr[2003][2003];
int counts[2][2003][2003] = {{{0}}};
int m, n;

inline void reduceCounts(int t, int r, int c){
#pragma omp critial
  {
    counts[t][r-1][c-1]--;
    counts[t][r-1][c]--;
    counts[t][r-1][c+1]--;
    counts[t][r][c-1]--;
    counts[t][r][c+1]--;
    counts[t][r+1][c-1]--;
    counts[t][r+1][c]--;
    counts[t][r+1][c+1]--;
  }
}

inline void increaseCounts(int t, int r, int c){
#pragma omp critial
  {
    counts[t][r-1][c-1]++;
    counts[t][r-1][c]++;
    counts[t][r-1][c+1]++;
    counts[t][r][c-1]++;
    counts[t][r][c+1]++;
    counts[t][r+1][c-1]++;
    counts[t][r+1][c]++;
    counts[t][r+1][c+1]++;
  }  
}

int main(){
  char s[2004];
  scanf("%d %d\n", &n, &m);
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
    }
  */
  //  omp_set_num_threads(20);

#pragma omp parallel for collapse(2)
  //first: calculate initial count
  for(int i = 1; i <= n; i++){
    for(int j = 1; j <= n; j++){
      int count = 0;
      if(arr[i-1][j])
	count++;
      if(arr[i-1][j+1])
	count++;
      if(arr[i-1][j-1])
	count++;	
      if(arr[i][j-1])
	count++;
      if(arr[i][j+1])
	count++;
      if(arr[i+1][j+1])
	count++;
      if(arr[i+1][j])
	count++;	
      if(arr[i+1][j-1])
	count++;
      counts[1][i][j] = count;
    }
  }
  
  for(int t = 1; t <= m; t++){
    //fprintf(stderr, "turn %d\n", t);
#pragma omp parallel
    {
#pragma omp for collapse(2)
    for(int r = 1; r <= n; r++)
      for(int c = 1; c <= n; c++)
	counts[(t+1)%2][r][c] = counts[t%2][r][c];
#pragma omp for collapse(2)
    for(int i = 1; i <= n; i++){
      for(int j = 1; j <= n; j++){
	if((arr[i][j] == 1) && !((counts[t%2][i][j] == 2) || (counts[t%2][i][j] == 3))){
	  arr[i][j] = 0;
	  reduceCounts((t+1)%2, i, j);	    
	} else if((arr[i][j] == 0) && (counts[t%2][i][j] == 3)){
	  arr[i][j] = 1;
	  increaseCounts((t+1)%2, i, j);
	}
	//fprintf(stderr, "turn %d = %d %d = %d\n", t , i, j , arr[i][j]);
      }

    }
  }
}
for(int i = 1; i <= n; i++){
  for(int j = 1; j <= n; j++)
    printf("%d", arr[i][j]);
  printf("\n");
 }
return 0;
}

