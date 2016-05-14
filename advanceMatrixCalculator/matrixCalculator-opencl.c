#define CL_USE_DEPRECATED_OPENCL_2_0_APIS 

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <CL/cl.h>
//#include <omp.h>
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


#ifdef DEBUG
#define checkSuccess()\
  if(status != CL_SUCCESS)				\
printf("at line %d: not success, error number %d\n",  __LINE__, status)
#else
#define checkSuccess() 
#endif


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
  //printf("The size of kernel source is %zu\n", kernelLength);
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
  //printf("Build program completes\n");
  /* createkernel */
  mul_kernel = clCreateKernel(program, "multiply", &status);
  add_kernel = clCreateKernel(program, "add", &status);
  checkSuccess();
  //printf("Build kernel completes\n");
}



void multiply(int N, UINT A[][MAXN], UINT B[][MAXN], UINT C[][MAXN]) {
cl_mem bufferA, bufferB, bufferC;  
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
  //printf("Build buffers completes\n");
  /* setarg */
  status = clSetKernelArg(mul_kernel, 0, sizeof(int), (void*)&N);
  checkSuccess();  
  status = clSetKernelArg(mul_kernel, 1, sizeof(cl_mem), (void*)&bufferA);
  checkSuccess();
  status = clSetKernelArg(mul_kernel, 2, sizeof(cl_mem), (void*)&bufferB);
  checkSuccess();
  status = clSetKernelArg(mul_kernel, 3, sizeof(cl_mem), (void*)&bufferC);
  checkSuccess();
  //printf("Set kernel arguments completes\n");
  /* setshape */
  size_t globalThreads[] = {(size_t)N};
  size_t localThreads[] = {1};
  status = clEnqueueNDRangeKernel(commandQueue, mul_kernel, 1, NULL, 
								  globalThreads, localThreads, 
								  0, NULL, NULL);
  checkSuccess();
  //printf("Specify the shape of the domain completes.\n");
  /* getcvector */
  clEnqueueReadBuffer(commandQueue, bufferC, CL_TRUE, 
					  0, MAXN*MAXN * sizeof(UINT), C, 
					  0, NULL, NULL);
  clFinish(commandQueue);

  clReleaseMemObject(bufferA);	
  clReleaseMemObject(bufferB);
  clReleaseMemObject(bufferC);
}
void add(int N, UINT A[][MAXN], UINT B[][MAXN], UINT C[][MAXN]) {
cl_mem bufferA, bufferB, bufferC;  
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
  //printf("Build buffers completes\n");
  /* setarg */
  status = clSetKernelArg(add_kernel, 0, sizeof(int), (void*)&N);
  checkSuccess();  
  status = clSetKernelArg(add_kernel, 1, sizeof(cl_mem), (void*)&bufferA);
  checkSuccess();
  status = clSetKernelArg(add_kernel, 2, sizeof(cl_mem), (void*)&bufferB);
  checkSuccess();
  status = clSetKernelArg(add_kernel, 3, sizeof(cl_mem), (void*)&bufferC);
  checkSuccess();
  //printf("Set kernel arguments completes\n");
  /* setshape */
  size_t globalThreads[] = {(size_t)N};
  size_t localThreads[] = {1};
  status = clEnqueueNDRangeKernel(commandQueue, add_kernel, 1, NULL, 
								  globalThreads, localThreads, 
								  0, NULL, NULL);
  checkSuccess();
  //printf("Specify the shape of the domain completes.\n");
  /* getcvector */
  clEnqueueReadBuffer(commandQueue, bufferC, CL_TRUE, 
					  0, MAXN*MAXN * sizeof(UINT), C, 
					  0, NULL, NULL);
  clFinish(commandQueue);

  clReleaseMemObject(bufferA);	
  clReleaseMemObject(bufferB);
  clReleaseMemObject(bufferC);
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


UINT IN[26][MAXN][MAXN], TMP[26][MAXN][MAXN];
int M, N, Q, S[26];
char qstr[100][30];
int main() {
  scanf("%d %d", &M, &N);
  for (int i = 0; i < M; i++) {
    scanf("%d", &S[i]);
    rand_gen(S[i], N, IN[i]);
  }

  initOpenCL();
  
  scanf("%d", &Q);
  
  for(int i = 0; i < Q; i++){
	scanf("%s", qstr[i]);
	//printf("read quest %s\n", qstr[i]);
	//TODO: use more better way to do it
	int tmpindex = 0;
	int nowmulindex = 0;
	int toMultiply[30][30];
	for(int x = 0; x < 30; x++)
	  for(int y = 0; y < 30; y++)
		toMultiply[x][y] = -1;
	for(int j = 0; j < strlen(qstr[i]); j++){
	  char c = qstr[i][j];
	  if(c == '+'){
		tmpindex++;
		nowmulindex = 0;
	  } else {
		toMultiply[tmpindex][nowmulindex++] = c - 'A';
		if(j == strlen(qstr[i])-1){
		  tmpindex++;
		  nowmulindex = 0;
		}
	  }
	}

	#pragma omp parallel for
	for(int j = 0; j < tmpindex; j++){
	  memcpy(TMP[j], IN[toMultiply[j][0]], MAXN*MAXN*sizeof(UINT));
	  int nowmulindex = 1;	  
	  while(toMultiply[j][nowmulindex] != -1){
		//printf("mul %d\n", nowmulindex);
		multiply(N, TMP[j], IN[toMultiply[j][nowmulindex]], TMP[j]);
		nowmulindex++;
	  }
	}

	for(int j = 0; j < tmpindex-1; j++){
	  add(N, TMP[j], TMP[j+1], TMP[j+1]);
	}
	if(tmpindex == 0)
	  printf("%u\n", signature(N, TMP[0]));
	else
	  printf("%u\n", signature(N, TMP[tmpindex-1]));
  }

  clReleaseContext(context);	
  clReleaseCommandQueue(commandQueue);
  clReleaseProgram(program);
  clReleaseKernel(add_kernel);
  clReleaseKernel(mul_kernel);
  
  return 0;
}

