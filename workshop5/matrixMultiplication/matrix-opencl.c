#define CL_USE_DEPRECATED_OPENCL_2_0_APIS 

#include <stdio.h>
#include <stdint.h>
#include <CL/cl.h>
// #define DEBUG
#define UINT uint32_t
#define MAXN 1025
#define MAXGPU 10
#define MAXK 1024

int N;
uint32_t S1, S2;


/*void multiply(int N, UINT A[][2048], UINT B[][2048], UINT C[][2048]) {
  for (int i = 0; i < N; i++) {
  for (int j = 0; j < N; j++) {
  UINT sum = 0;    // overflow, let it go.
  for (int k = 0; k < N; k++)
  sum += A[i][k] * B[k][j];
  C[i][j] = sum;
  }
  }
  }*/
void rand_gen(UINT c, int N, UINT A[][N]) {
UINT x = 2, n = N*N;
for (int i = 0; i < N; i++) {
for (int j = 0; j < N; j++) {
x = (x * x + c + i + j)%n;
A[i][j] = x;
}
}
}
void print_matrix(int N, UINT A[][N]) {
for (int i = 0; i < N; i++) {
fprintf(stderr, "[");
for (int j = 0; j < N; j++)
  fprintf(stderr, " %u", A[i][j]);
fprintf(stderr, " ]\n");
}
}
UINT signature(int N, UINT A[][N]) {
UINT h = 0;
for (int i = 0; i < N; i++) {
for (int j = 0; j < N; j++)
  h = (h + A[i][j]) * 2654435761LU;
}
return h;
}

cl_int status;
cl_platform_id platform_id;
cl_uint platform_id_got;
cl_device_id GPU[MAXGPU];
cl_uint GPU_id_got;
cl_context context;
cl_program program;
cl_command_queue commandQueue;
cl_kernel kernel;
void initOpenCL(){
status = clGetPlatformIDs(1, &platform_id, 
			    &platform_id_got);
//assert(status == CL_SUCCESS && platform_id_got == 1);
//printf("%d platform found\n", platform_id_got);

status = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 
			  MAXGPU, GPU, &GPU_id_got);
if(status != CL_SUCCESS){
//printf("no success\n");
}
//printf("There are %d GPU devices\n", GPU_id_got); 
/* getcontext */
context = 
  clCreateContext(NULL, GPU_id_got, GPU, NULL, NULL, 
		    &status);
if(status != CL_SUCCESS){
//printf("no success\n");
}
/* commandqueue */
commandQueue =
  clCreateCommandQueue(context, GPU[0], 0, &status);
if(status != CL_SUCCESS){
//printf("no success\n");
}
/* kernelsource */
FILE *kernelfp = fopen("matrixmul.cl", "r");
//assert(kernelfp != NULL);
char kernelBuffer[MAXK];
const char *constKernelSource = kernelBuffer;
size_t kernelLength = 
  fread(kernelBuffer, 1, MAXK, kernelfp);
//printf("The size of kernel source is %zu\n", kernelLength);
program =
  clCreateProgramWithSource(context, 1, &constKernelSource, 
			      &kernelLength, &status);
if(status != CL_SUCCESS){
//printf("no success\n");
}
/* buildprogram */
status = 
  clBuildProgram(program, GPU_id_got, GPU, NULL, NULL, 
		   NULL);
if(status != CL_SUCCESS){
if(status == CL_BUILD_PROGRAM_FAILURE){
size_t log_size;
clGetProgramBuildInfo(program, GPU[0], CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
// Allocate memory for the log
char *log = (char *) malloc(log_size);
// Get the log
clGetProgramBuildInfo(program, GPU[0], CL_PROGRAM_BUILD_LOG, log_size, log, NULL);
// Print the log
//printf("error: %s\n", log);
}

exit(1);
}
//printf("Build program completes\n");
/* createkernel */
kernel = clCreateKernel(program, "multiply", &status);
if(status != CL_SUCCESS){
//printf("not success \n");
}
//printf("Build kernel completes\n");
}

cl_mem bufferA, bufferB, bufferC;
cl_uint* A, *B, *C;
void executeOpenCL(){
/* createbuffer */
bufferA = 
  clCreateBuffer(context, 
		   CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
		   N*N * sizeof(cl_uint), A, &status);
if(status != CL_SUCCESS){
//printf("no success\n");
}
bufferB = 
  clCreateBuffer(context, 
		   CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
		   N*N * sizeof(cl_uint), B, &status);
if(status != CL_SUCCESS){
//printf("no success\n");
}
bufferC = 
  clCreateBuffer(context, 
		   CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR,
		   N*N * sizeof(cl_uint), C, &status);
if(status != CL_SUCCESS){
//printf("no success\n");
}
//printf("Build buffers completes\n");
/* setarg */
status = clSetKernelArg(kernel, 0, sizeof(int), 
			  (void*)&N);
status = clSetKernelArg(kernel, 1, sizeof(cl_mem), 
			  (void*)&bufferA);
if(status != CL_SUCCESS){
//printf("no success\n");
}

status = clSetKernelArg(kernel, 2, sizeof(cl_mem), 
			  (void*)&bufferB);
if(status != CL_SUCCESS){
//printf("no success\n");
}
status = clSetKernelArg(kernel, 3, sizeof(cl_mem), 
			  (void*)&bufferC);
if(status != CL_SUCCESS){
//printf("no success\n");
}

//printf("Set kernel arguments completes\n");
/* setshape */
size_t globalThreads[] = {(size_t)N};
size_t localThreads[] = {1};
status = 
  clEnqueueNDRangeKernel(commandQueue, kernel, 1, NULL, 
			   globalThreads, localThreads, 
			   0, NULL, NULL);
if(status != CL_SUCCESS){
//printf("no success\n");
}
//printf("Specify the shape of the domain completes.\n");
/* getcvector */
clEnqueueReadBuffer(commandQueue, bufferC, CL_TRUE, 
		      0, N*N * sizeof(cl_uint), C, 
		      0, NULL, NULL);
//printf("Kernel execution completes.\n");
}


int main() {
initOpenCL();

scanf("%d %u %u", &N, &S1, &S2);
/* vector */
A  = (cl_uint*)malloc(N *N * sizeof(cl_uint));
B = (cl_uint*)malloc(N *N* sizeof(cl_uint));
C = (cl_uint*)malloc(N *N* sizeof(cl_uint));

for(int i = 0; i < N; i++)
  for(int j = 0; j < N; j++)
    C[i*N+j] = 0;

rand_gen(S1, N, A);
rand_gen(S2, N, B);

executeOpenCL();

//print_matrix(N, A);
//print_matrix(N, B);
// print_matrix(N, C);
 printf("%u\n", signature(N, C));

 /*free(A);	
   free(B);
   free(C);
   clReleaseContext(context);	
   clReleaseCommandQueue(commandQueue);
   clReleaseProgram(program);
   clReleaseKernel(kernel);
   clReleaseMemObject(bufferA);	
   clReleaseMemObject(bufferB);
   clReleaseMemObject(bufferC);*/
  
 return 0;
}
