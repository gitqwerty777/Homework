__kernel void goNextState(int n, int t, int MAXN, __global unsigned int *arr, __global unsigned int* counts){
  int i, j=0, k;
  i = get_global_id(0);
  i++;//1~n
  //j = get_global_id(1);//TODO:
  printf("%d, %d, %d, %d\n", t, i, j, counts[0]);


  for(j = 1; j <= n; j++){
    //TODO: opencl critical?
    if((arr[i*MAXN+j] == 1) && !((counts[(t%2)*MAXN*MAXN+i*MAXN+j] == 2) || (counts[(t%2)*MAXN*MAXN+i*MAXN+j] == 3))){
      printf("%d %d decrease", i, j);
      arr[i*MAXN+j] = 0;
      counts[((t+1)%2)*MAXN*MAXN+(i-1)*MAXN+j-1]--;
      counts[((t+1)%2)*MAXN*MAXN+(i-1)*MAXN+j]--;
      counts[((t+1)%2)*MAXN*MAXN+(i-1)*MAXN+j+1]--;
      counts[((t+1)%2)*MAXN*MAXN+i*MAXN+j-1]--;
      counts[((t+1)%2)*MAXN*MAXN+i*MAXN+j+1]--;
      counts[((t+1)%2)*MAXN*MAXN+(i+1)*MAXN+j-1]--;
      counts[((t+1)%2)*MAXN*MAXN+(i+1)+j]--;
      counts[((t+1)%2)*MAXN*MAXN+(i+1)*MAXN+j+1]--;
    } else if((arr[i*MAXN+j] == 0) && (counts[(t%2)*MAXN*MAXN+i*MAXN+j] == 3)){
      printf("%d %d increase", i, j);
      arr[i*MAXN+j] = 1;
      counts[((t+1)%2)*MAXN*MAXN+(i-1)*MAXN+j-1]++;
      counts[((t+1)%2)*MAXN*MAXN+(i-1)*MAXN+j]++;
      counts[((t+1)%2)*MAXN*MAXN+(i-1)*MAXN+j+1]++;
      counts[((t+1)%2)*MAXN*MAXN+i*MAXN+j-1]++;
      counts[((t+1)%2)*MAXN*MAXN+i*MAXN+j+1]++;
      counts[((t+1)%2)*MAXN*MAXN+(i+1)*MAXN+j-1]++;
      counts[((t+1)%2)*MAXN*MAXN+(i+1)*MAXN+j]++;
      counts[((t+1)%2)*MAXN*MAXN+(i+1)*MAXN+j+1]++;
    }
  }
}
