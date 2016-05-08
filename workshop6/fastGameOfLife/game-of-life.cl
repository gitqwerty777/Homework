__kernel void goNextState(int n, int tt, int MAXN, __global unsigned int *arr){
  int i, j=0, k;
  i = get_global_id(0);
  i++;//1~n
  j = get_global_id(1);
  j++;

  int t;
  for(t = 1 ; t <= tt; t++){
    int count = 0;    
    //printf("%d, %d turn %d\n", i, j , t);
    if(arr[((t+1)%2)*MAXN*MAXN+(i-1)*MAXN+j])
      count++;
    if(arr[((t+1)%2)*MAXN*MAXN+(i-1)*MAXN+j+1])
      count++;
    if(arr[((t+1)%2)*MAXN*MAXN+(i-1)*MAXN+j-1])
      count++;    
    if(arr[((t+1)%2)*MAXN*MAXN+i*MAXN+j-1])
      count++;
    if(arr[((t+1)%2)*MAXN*MAXN+i*MAXN+j+1])
      count++;
    if(arr[((t+1)%2)*MAXN*MAXN+(i+1)*MAXN+j+1])
      count++;
    if(arr[((t+1)%2)*MAXN*MAXN+(i+1)*MAXN+j])
      count++;    
    if(arr[((t+1)%2)*MAXN*MAXN+(i+1)*MAXN+j-1])
      count++;

    if((arr[((t+1)%2)*MAXN*MAXN+i*MAXN+j] == 1) && !((count == 2) || (count == 3))){
      arr[((t)%2)*MAXN*MAXN+i*MAXN+j] = 0;
      //counts[((t+1)%2)*MAXN*MAXN+i*MAXN+j] = 0;
    } else if((arr[((t+1)%2)*MAXN*MAXN+i*MAXN+j] == 0) && (count == 3)){
      arr[((t)%2)*MAXN*MAXN+i*MAXN+j] = 1;    
      //counts[((t+1)%2)*MAXN*MAXN+i*MAXN+j] = 1;
    } else{
      arr[((t)%2)*MAXN*MAXN+i*MAXN+j] = arr[((t+1)%2)*MAXN*MAXN+i*MAXN+j];    
    }

    barrier ( CLK_GLOBAL_MEM_FENCE );
  }
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

