#define MAXN 2003
#define BSIDE 16

__kernel void goNextState(int n, int t, __global unsigned int *arr){
  int i, j;
  i = get_global_id(0)+1;
  j = get_global_id(1)+1;

  __local unsigned int sur[BSIDE][BSIDE];//surrounding
  int li;
  li = get_local_id(0);
  int lj;
  lj = get_local_id(1);
  
  //int t;
  //  for(t = 1 ; t <= tt; t++){
  int count = 0;    
  //printf("%d, %d turn %d\n", i, j , t);
  /*
  int original = arr[((t+1)%2)*MAXN*MAXN+(i)*MAXN+j];
  printf("%d, %d = %d\n", i, j, original);
  if(i <= n && j <= n)
    sur[li][lj] = original;
  
  barrier(CLK_LOCAL_MEM_FENCE);
  
  if(i < n && j < n && li != 0 && li != BSIDE-1 && lj != 0 && lj != BSIDE-1){
    //use add
    if(sur[li-1][lj])
      count++;
    if(sur[li-1][lj+1])
      count++;
    if(sur[li-1][lj-1])
      count++;    
    if(sur[li][lj+1])
      count++;
    if(sur[li][lj-1])
      count++;
    if(sur[li+1][lj+1])
      count++;
    if(sur[li+1][lj])
      count++;    
    if(sur[li+1][lj-1])
      count++;

    if((original == 1) && !((count == 2) || (count == 3))){
      arr[((t)%2)*MAXN*MAXN+i*MAXN+j] = 0;
      //counts[((t+1)%2)*MAXN*MAXN+i*MAXN+j] = 0;
    } else if((original == 0) && (count == 3)){
      arr[((t)%2)*MAXN*MAXN+i*MAXN+j] = 1;    
      //counts[((t+1)%2)*MAXN*MAXN+i*MAXN+j] = 1;
    } else{
      arr[((t)%2)*MAXN*MAXN+i*MAXN+j] = original;
    }
  }
  */
  //    barrier ( CLK_GLOBAL_MEM_FENCE );
  //  }
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
