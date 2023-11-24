#define CL_USE_DEPRECATED_OPENCL_2_0_APIS 

#include <stdio.h>
#include <stdint.h>
#include <CL/cl.h>
#define MAXGPU 10
#define MAXK 1024000
#define MAXN 1024

int N;

cl_int status;

cl_platform_id platform_id;
cl_uint platform_id_got;
cl_device_id GPU[MAXGPU];
cl_uint GPU_id_got;

cl_context context;
cl_program program;
cl_command_queue commandQueue;
cl_kernel kernel;


#ifdef DEBUG
#define checkSuccess()\
  printf("at line %d: not success\n",  __LINE__);
#endif

void initOpenCL(char* kernelFileName){
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
  FILE *kernelfp = fopen("./fastGameOfLife/game-of-life.cl", "r");
  char kernelBuffer[MAXK];
  const char *constKernelSource = kernelBuffer;
  size_t kernelLength = fread(kernelBuffer, 1, MAXK, kernelfp);
  fclose(kernelfp);
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
      printf("%s", log);
    }
  }
  //printf("Build program completes\n");
  /* createkernel */
  kernel = clCreateKernel(program, "goNextState", &status);
  checkSuccess();
  //printf("Build kernel completes\n");
}

cl_mem bufferA, bufferB, bufferC;
cl_uint* A, *B, *C;
void executeOpenCL(){
  /* createbuffer */
  bufferA = clCreateBuffer(context, 
			   CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
			   N*N * sizeof(cl_uint), A, &status);
  checkSuccess();
  bufferB = clCreateBuffer(context, 
			   CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
			   N*N * sizeof(cl_uint), B, &status);
  checkSuccess();
  bufferC = clCreateBuffer(context, 
			   CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR,
			   N*N * sizeof(cl_uint), C, &status);
  checkSuccess();
  printf("Build buffers completes\n");
  /* setarg */
  status = clSetKernelArg(kernel, 0, sizeof(int), (void*)N);
  checkSuccess();  
  status = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*)&bufferA);
  checkSuccess();
  status = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void*)&bufferB);
  checkSuccess();
  status = clSetKernelArg(kernel, 3, sizeof(cl_mem), (void*)&bufferC);
  checkSuccess();
  printf("Set kernel arguments completes\n");
  /* setshape */
  size_t globalThreads[] = {1024, 1024};
  size_t localThreads[] = {64, 64};
  status = clEnqueueNDRangeKernel(commandQueue, kernel, 2, NULL, 
				  globalThreads, localThreads, 
				  0, NULL, NULL);
  if(status != CL_SUCCESS){
    fprintf(stderr, "CL_INVALID_WORK_GROUP_SIZE error:%d\n", status);
  }
  checkSuccess();
  printf("Specify the shape of the domain completes.\n");
  /* getcvector */
  clEnqueueReadBuffer(commandQueue, bufferC, CL_TRUE, 
		      0, N*N * sizeof(cl_uint), C, 
		      0, NULL, NULL);
  printf("Kernel execution completes.\n");
}

int main() {
  char s[1000];
  scanf("%s", s);
  
  initOpenCL(s);
  A  = (cl_uint*)malloc(N *N * sizeof(cl_uint));
  B = (cl_uint*)malloc(N *N* sizeof(cl_uint));
  C = (cl_uint*)malloc(N *N* sizeof(cl_uint));
  
  executeOpenCL();
  
  /*



  free(A);	
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
