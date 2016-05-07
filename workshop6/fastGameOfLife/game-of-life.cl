__kernel void goNextState(int n, int t, __global unsigned int *arr, __global unsigned int* counts){
  int i, j=0, k;
  i = get_global_id(0);
  //j = get_global_id(1);

  printf("%d, %d, %d, %d\n", t, i, j, counts[0]);
  
  /*int r, c;
  //counts[(t+1)%2][r][c] = counts[t%2][r][c];
  //TODO: opencl critical?
  if((arr[i][j] == 1) && !((counts[t%2][i][j] == 2) || (counts[t%2][i][j] == 3))){
    arr[i][j] = 0;
    counts[t][i-1][j-1]--;
    counts[t][i-1][j]--;
    counts[t][i-1][j+1]--;
    counts[t][i][j-1]--;
    counts[t][i][j+1]--;
    counts[t][i+1][j-1]--;
    counts[t][i+1][j]--;
    counts[t][i+1][j+1]--;
  } else if((arr[i][j] == 0) && (counts[t%2][i][j] == 3)){
    arr[i][j] = 1;
    counts[t][i-1][j-1]++;
    counts[t][i-1][j]++;
    counts[t][i-1][j+1]++;
    counts[t][i][j-1]++;
    counts[t][i][j+1]++;
    counts[t][i+1][j-1]++;
    counts[t][i+1][j]++;
    counts[t][i+1][j+1]++;
    }*/
}
