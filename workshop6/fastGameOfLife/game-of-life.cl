#define MAXN 2003
#define BSIDE 8

__kernel void goNextState(int n, int t, __global unsigned int *arr){
  int i = get_global_id(0)+1;
  int j = get_global_id(1)+1;
  int li = get_local_id(0)+1;
  int lj = get_local_id(1)+1;
  
  __local unsigned int sur[BSIDE+2][BSIDE+2];//surroundings
  
  int original = arr[((t+1)%2)*MAXN*MAXN+(i)*MAXN+j];
  if(i <= n && j <= n){
    sur[li][lj] = original;
    if(li == 1){
      sur[0][lj] = arr[((t+1)%2)*MAXN*MAXN+(i-1)*MAXN+j];
      if(lj == 1){//corner
	sur[0][0] = arr[((t+1)%2)*MAXN*MAXN+(i-1)*MAXN+j-1];
      } else if(lj == BSIDE || j == n){
	sur[0][lj+1] = arr[((t+1)%2)*MAXN*MAXN+(i-1)*MAXN+j+1];
      }
    } else if(li == BSIDE || i == n){
      sur[li+1][lj] = arr[((t+1)%2)*MAXN*MAXN+(i+1)*MAXN+j];      
      if(lj == 1){
	sur[li+1][0] = arr[((t+1)%2)*MAXN*MAXN+(i+1)*MAXN+j-1];
      } else if(lj == BSIDE || j == n){
	sur[li+1][lj+1] = arr[((t+1)%2)*MAXN*MAXN+(i+1)*MAXN+j+1];
      }
    }

    if(lj == 1){
      sur[li][0] = arr[((t+1)%2)*MAXN*MAXN+i*MAXN+j-1];
    } else if(lj == BSIDE || j == n){
      sur[li][lj+1] = arr[((t+1)%2)*MAXN*MAXN+i*MAXN+j+1];
    }    
  }

  barrier(CLK_LOCAL_MEM_FENCE);

  if(!(i <= n && j <= n)) return;

  //printf("global (%d, %d), local (%d, %d)\n", i, j, li, lj);
  int count = 0;          
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
