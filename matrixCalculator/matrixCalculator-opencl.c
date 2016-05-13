#define CL_USE_DEPRECATED_OPENCL_2_0_APIS 

#include <stdio.h>
#include <stdint.h>
#include <CL/cl.h>
#define MAXGPU 10
#define MAXK 1024
#define UINT cl_uint
#define MAXN 1024

cl_int status;

cl_platform_id platform_id;
cl_uint platform_id_got;
cl_device_id GPU[MAXGPU];
cl_uint GPU_id_got;

cl_context context;
cl_program program;
cl_command_queue commandQueue;
cl_kernel mul_kernel, add_kernel;


inline void checkSuccess(){
#ifdef DEBUG
  printf("at line %d: not success\n",  __LINE__);
#endif
}


void initOpenCL(){
  status = clGetPlatformIDs(1, &platform_id, &platform_id_got);
  status = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, MAXGPU, GPU, &GPU_id_got);
  checkSuccess();
  //printf("There are %d GPU devices\n", GPU_id_got); 
  /* getcontext */
  context = clCreateContext(NULL, GPU_id_got, GPU, NULL, NULL, &status);
  checkSuccess();
  /* commandqueue */
  commandQueue = clCreateCommandQueue(context, GPU[0], 0, &status);
  checkSuccess();
  /* kernelsource */
  FILE *kernelfp = fopen("matrix-lib.cl", "r");
  char kernelBuffer[MAXK];
  const char *constKernelSource = kernelBuffer;
  size_t kernelLength = fread(kernelBuffer, 1, MAXK, kernelfp);
  printf("The size of kernel source is %zu\n", kernelLength);
  program = clCreateProgramWithSource(context, 1, &constKernelSource, &kernelLength, &status);
  checkSuccess();
  /* buildprogram */
  status = clBuildProgram(program, GPU_id_got, GPU, NULL, NULL, NULL);
  /* check if .cl file has compile error or not */
  if(status != CL_SUCCESS){
    if(status == CL_BUILD_PROGRAM_FAILURE){
      size_t log_size;
      clGetProgramBuildInfo(program, GPU[0], CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
      // Allocate memory for the log
      char *log = (char *) malloc(log_size);
      // Get the log
      clGetProgramBuildInfo(program, GPU[0], CL_PROGRAM_BUILD_LOG, log_size, log, NULL);
      // Print the log
      printf("error: %s\n", log);
    }
    exit(1);
  }
  printf("Build program completes\n");
  /* createkernel */
  mul_kernel = clCreateKernel(program, "multiply", &status);
  add_kernel = clCreateKernel(program, "add", &status);
  checkSuccess();
  printf("Build kernel completes\n");
}

cl_mem bufferA, bufferB, bufferC;

void multiply(int N, UINT A[][MAXN], UINT B[][MAXN], UINT C[][MAXN]) {
  bufferA = clCreateBuffer(context, 
			   CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
			   MAXN*MAXN * sizeof(UINT), A, &status);
  checkSuccess();
  bufferB = clCreateBuffer(context, 
			   CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
			   MAXN*MAXN * sizeof(UINT), B, &status);
  checkSuccess();
  bufferC = clCreateBuffer(context, 
			   CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR,
			   MAXN*MAXN * sizeof(UINT), C, &status);
  checkSuccess();
  printf("Build buffers completes\n");
  /* setarg */
  status = clSetKernelArg(mul_kernel, 0, sizeof(int), (void*)&N);
  checkSuccess();  
  status = clSetKernelArg(mul_kernel, 1, sizeof(cl_mem), (void*)&bufferA);
  checkSuccess();
  status = clSetKernelArg(mul_kernel, 2, sizeof(cl_mem), (void*)&bufferB);
  checkSuccess();
  status = clSetKernelArg(mul_kernel, 3, sizeof(cl_mem), (void*)&bufferC);
  checkSuccess();
  printf("Set kernel arguments completes\n");
  /* setshape */
  size_t globalThreads[] = {(size_t)N};
  size_t localThreads[] = {1};
  status = clEnqueueNDRangeKernel(commandQueue, mul_kernel, 1, NULL, 
				  globalThreads, localThreads, 
				  0, NULL, NULL);
  checkSuccess();
  printf("Specify the shape of the domain completes.\n");
  /* getcvector */
  clEnqueueReadBuffer(commandQueue, bufferC, CL_TRUE, 
		      0, MAXN*MAXN * sizeof(UINT), C, 
		      0, NULL, NULL);
  clFinish(commandQueue);
}
void add(int N, UINT A[][MAXN], UINT B[][MAXN], UINT C[][MAXN]) {
  bufferA = clCreateBuffer(context, 
			   CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
			   MAXN*MAXN * sizeof(UINT), A, &status);
  checkSuccess();
  bufferB = clCreateBuffer(context, 
			   CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
			   MAXN*MAXN * sizeof(UINT), B, &status);
  checkSuccess();
  bufferC = clCreateBuffer(context, 
			   CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR,
			   MAXN*MAXN * sizeof(UINT), C, &status);
  checkSuccess();
  printf("Build buffers completes\n");
  /* setarg */
  status = clSetKernelArg(add_kernel, 0, sizeof(int), (void*)&N);
  checkSuccess();  
  status = clSetKernelArg(add_kernel, 1, sizeof(cl_mem), (void*)&bufferA);
  checkSuccess();
  status = clSetKernelArg(add_kernel, 2, sizeof(cl_mem), (void*)&bufferB);
  checkSuccess();
  status = clSetKernelArg(add_kernel, 3, sizeof(cl_mem), (void*)&bufferC);
  checkSuccess();
  printf("Set kernel arguments completes\n");
  /* setshape */
  size_t globalThreads[] = {(size_t)N};
  size_t localThreads[] = {1};
  status = clEnqueueNDRangeKernel(commandQueue, add_kernel, 1, NULL, 
				  globalThreads, localThreads, 
				  0, NULL, NULL);
  checkSuccess();
  printf("Specify the shape of the domain completes.\n");
  /* getcvector */
  clEnqueueReadBuffer(commandQueue, bufferC, CL_TRUE, 
		      0, MAXN*MAXN * sizeof(UINT), C, 
		      0, NULL, NULL);
  clFinish(commandQueue);
}
void rand_gen(UINT c, int N, UINT A[][MAXN]) {
  UINT x = 2, n = N*N;
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      x = (x * x + c + i + j)%n;
      A[i][j] = x;
    }
  }
}
void print_matrix(int N, UINT A[][MAXN]) {
  for (int i = 0; i < N; i++) {
    fprintf(stderr, "[");
    for (int j = 0; j < N; j++)
      fprintf(stderr, " %u", A[i][j]);
    fprintf(stderr, " ]\n");
  }
}
UINT signature(int N, UINT A[][MAXN]) {
  UINT h = 0;
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++)
      h = (h + A[i][j]) * 2654435761LU;
  }
  return h;
}


UINT IN[6][MAXN][MAXN], TMP[6][MAXN][MAXN];
int N, S[6];
int main() {
  scanf("%d", &N);
  for (int i = 0; i < 6; i++) {
    scanf("%d", &S[i]);
    rand_gen(S[i], N, IN[i]);
  }

  initOpenCL();
  //executeOpenCL();//execute mul, add


  // AB
  multiply(N, IN[0], IN[1], TMP[0]);
  // CD
  multiply(N, IN[2], IN[3], TMP[1]);
  // AB+CD
  add(N, TMP[0], TMP[1], TMP[2]);
  printf("%u\n", signature(N, TMP[2]));
 
  // ABE
  multiply(N, TMP[0], IN[4], TMP[3]);
  // CDF
  multiply(N, TMP[1], IN[5], TMP[4]);
  // ABE+CDF
  add(N, TMP[3], TMP[4], TMP[5]);
  printf("%u\n", signature(N, TMP[5]));


  clReleaseContext(context);	
  clReleaseCommandQueue(commandQueue);
  clReleaseProgram(program);
  clReleaseKernel(add_kernel);
  clReleaseKernel(mul_kernel);
  clReleaseMemObject(bufferA);	
  clReleaseMemObject(bufferB);
  clReleaseMemObject(bufferC);
  
  return 0;
}

