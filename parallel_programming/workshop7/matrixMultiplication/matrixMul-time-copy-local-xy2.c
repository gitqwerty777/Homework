#define CL_USE_DEPRECATED_OPENCL_2_0_APIS 

#include <stdio.h>
#include <stdint.h>
#include <CL/cl.h>
// #define DEBUG
#define UINT uint32_t
#define MAXN 1025
#define MAXGPU 10
#define MAXK 1024
#define DEVICENUM 2

int N;
uint32_t S1, S2;

cl_uint A[N][N], B[N][N], C[N][N], D[N][N], E[N][N], F[N][N];
cl_uint tmp[4][N][N];


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
  cl_command_queue commandQueue[DEVICENUM];
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

  cl_context context = 
    clCreateContext(NULL, DEVICENUM, GPU, NULL, NULL, 
					&status);
  //assert(status == CL_SUCCESS);
  /* commandqueue */
  cl_command_queue commandQueue[DEVICENUM];
  for (int device = 0; device < DEVICENUM; device++) {
    commandQueue[device] = 
      clCreateCommandQueue(context, GPU[device],
						   CL_QUEUE_PROFILING_ENABLE, 
						   &status);
    //assert(status == CL_SUCCESS);
  }
  /* kernelsource */
  FILE *kernelfp = fopen(argv[1], "r");
  //assert(kernelfp != NULL);
  char kernelBuffer[MAXK];
  const char *constKernelSource = kernelBuffer;
  size_t kernelLength = 
    fread(kernelBuffer, 1, MAXK, kernelfp);
  printf("The size of kernel source is %zu\n", kernelLength);
  cl_program program =
    clCreateProgramWithSource(context, 1, &constKernelSource, 
							  &kernelLength, &status);
  //assert(status == CL_SUCCESS);
  /* buildprogram */
  status = 
    clBuildProgram(program, DEVICENUM, GPU, NULL, 
				   NULL, NULL);
  if (status != CL_SUCCESS) {
    char log[MAXLOG];
    size_t logLength;
    for (int device = 0; device < DEVICENUM; device++) {
      clGetProgramBuildInfo(program, GPU[device], 
							CL_PROGRAM_BUILD_LOG,
							MAXLOG, log, &logLength);
      puts(log);
    }
    exit(-1);
  }
  printf("Build program completes\n");
  /* createkernel */
  cl_kernel kernel = clCreateKernel(program, "multiply", &status);
  //assert(status == CL_SUCCESS);
  printf("Build kernel completes\n");
  /* vector */
  for (int i = 0; i < N; i++) 
    for (int j = 0; j < N; j++) {
      A[i][j] = i + j;
      B[i][j] = i - j;
	  C[i][j] = A[i][j]+B[i][j];
	  D[i][j] = A[i][j]-B[i][j];
	  E[i][j] = i*C[i][j];
	  F[i][j] = j*C[i][j];	  
    }
}

cl_mem bufferA[2], bufferB[2], bufferC[2];
void executeOpenCL(int device){
  /* createbuffer */
  bufferA[device] = 
	clCreateBuffer(context, 
				   CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
				   N*N * sizeof(cl_uint), A, &status);
  if(status != CL_SUCCESS){
	//printf("no success\n");
  }
  bufferB[device] = 
	clCreateBuffer(context, 
				   CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
				   N*N * sizeof(cl_uint), B, &status);
  if(status != CL_SUCCESS){
	//printf("no success\n");
  }
  bufferC[device] = 
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
						  (void*)&bufferA[device]);
  if(status != CL_SUCCESS){
	//printf("no success\n");
  }

  status = clSetKernelArg(kernel, 2, sizeof(cl_mem), 
						  (void*)&bufferB[device]);
  if(status != CL_SUCCESS){
	//printf("no success\n");
  }
  status = clSetKernelArg(kernel, 3, sizeof(cl_mem), 
						  (void*)&bufferC[device]);
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

  clEnqueueReadBuffer(commandQueue, bufferC[device], CL_TRUE, 
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

  free(A);	
	free(B);
	free(C);
	clReleaseContext(context);	
	clReleaseCommandQueue(commandQueue);
	clReleaseProgram(program);
	clReleaseKernel(kernel);
	clReleaseMemObject(bufferA[0]);
	clReleaseMemObject(bufferA[1]);	
	clReleaseMemObject(bufferB[0]);
	clReleaseMemObject(bufferB[1]);
	clReleaseMemObject(bufferC[0]);
	clReleaseMemObject(bufferC[1]);
  
	return 0;
}
