__kernel void multiply(int n, __global unsigned int *A, __global unsigned int* B, __global unsigned int* C){
 int i, j, k;
 i = get_global_id(0);

 for(j= 0; j < n; j++){
 C[i*n+j] = 0;

 for (k=0; k<n; k++){
 C[i*n+j] += A[i*n+k] * B[k*n+j];

 }
 }

}