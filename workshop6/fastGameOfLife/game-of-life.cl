__kernel void goNextState(int n, int t, int MAXN, __global unsigned int *arr, __global unsigned int* counts){
  int i, j=0, k;
  i = get_global_id(0);
  i++;//1~n
  j = get_global_id(1);
  j++;
  int count = 0;

  if(arr[(i-1)*MAXN+j])
    count++;
  if(arr[(i-1)*MAXN+j+1])
    count++;
  if(arr[(i-1)*MAXN+j-1])
    count++;    
  if(arr[i*MAXN+j-1])
    count++;
  if(arr[i*MAXN+j+1])
    count++;
  if(arr[(i+1)*MAXN+j+1])
    count++;
  if(arr[(i+1)*MAXN+j])
    count++;    
  if(arr[(i+1)*MAXN+j-1])
    count++;
  counts[(t%2)*MAXN*MAXN+i*MAXN+j] = count;
  
  if((arr[i*MAXN+j] == 1) && !((counts[(t%2)*MAXN*MAXN+i*MAXN+j] == 2) || (counts[(t%2)*MAXN*MAXN+i*MAXN+j] == 3))){
    arr[i*MAXN+j] = 0;
  } else if((arr[i*MAXN+j] == 0) && (counts[(t%2)*MAXN*MAXN+i*MAXN+j] == 3)){
    arr[i*MAXN+j] = 1;
  }
  /*for(j = 1; j <= n; j++){
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
    }*/
}
