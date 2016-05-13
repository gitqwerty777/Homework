#define CL_USE_DEPRECATED_OPENCL_2_0_APIS 

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <CL/cl.h>
#define MAXGPU 10
#define BSIDE 8
#define MAXK 40960
#define MAXN 2048

int m, n;
cl_mem countBuffer, arrayBuffer;
char arr[2*MAXN*MAXN];

//TODO: array with more dimension
//TODO: thread with more dimension
// use 1 dimension first(1 row)

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
#define checkSuccess()					\
  if(status != CL_SUCCESS)				\
    printf("at line %d: not success\n",  __LINE__);   
#else
#define checkSuccess() 
#endif						\


#ifdef DEBUG
#define writeLog(a, args...)			\
  printf("at line %d:"a,   __LINE__, ##args)
#else
#define writeLog(a, args...)
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
  FILE *kernelfp = fopen("game-of-life.cl", "r");
  char kernelBuffer[MAXK];
  const char *constKernelSource = kernelBuffer;
  size_t kernelLength = fread(kernelBuffer, 1, MAXK, kernelfp);
  writeLog("The size of kernel source is %zu\n", kernelLength);
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
      printf("%s\n", log);
    }
    exit(1);
  }
  writeLog("Build program completes\n");
  /* createkernel */
  kernel = clCreateKernel(program, "goNextState", &status);
  checkSuccess();
  writeLog("Build kernel completes\n");
}

void executeOpenCL(){
  /* createbuffer */
  //countBuffer = clCreateBuffer(context, 
  //			       CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
  //			       2*MAXN*MAXN* sizeof(cl_uint), counts, &status);
  //checkSuccess();
  arrayBuffer = clCreateBuffer(context, 
			       CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
			       2*MAXN*MAXN * sizeof(char), arr, &status);
  checkSuccess();
  writeLog("Build buffers completes\n");
  /* setarg */

  /* setshape */
  int globalN = n;
  //global size should be multiply of local size
  while(globalN % BSIDE){
    globalN++;
  }
  size_t globalThreads[] = {globalN, globalN};
  size_t localThreads[] = {BSIDE, BSIDE};

  int maxn = MAXN;
  status = clSetKernelArg(kernel, 0, sizeof(int), (void*)&n);
  checkSuccess();
  status = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void*)&arrayBuffer);
  checkSuccess();  
  
  for(int t = 1; t <= m; t++){
    status = clSetKernelArg(kernel, 1, sizeof(int), (void*)&t);
    checkSuccess();
    status = clEnqueueNDRangeKernel(commandQueue, kernel, 2, NULL, globalThreads, localThreads, 0, NULL, NULL);
    checkSuccess();
    clFinish(commandQueue);
  }

  clEnqueueReadBuffer(commandQueue, arrayBuffer, CL_TRUE, 
		      0, 2*MAXN*MAXN*sizeof(char), arr, 
		      0, NULL, NULL);
  writeLog("Specify the shape of the domain completes.\n");
  /* getcvector */
  /*  clEnqueueReadBuffer(commandQueue, arrayBuffer, CL_TRUE, 
      0, MAXN*MAXN*sizeof(cl_uint), arr, 
      0, NULL, NULL);
      clEnqueueReadBuffer(commandQueue, countBuffer, CL_TRUE, 
      0, 2*MAXN*MAXN*sizeof(cl_uint), counts, 
      0, NULL, NULL);*/
  writeLog("Kernel execution completes.\n");
}

int main() {
  char s[2004];
  scanf("%d %d\n", &n, &m);
  //  for(int i = 0; i < MAXN; i++)
  //    for(int j = 0; j < MAXN; j++)
  //      arr[i*MAXN+j] = 0;
  
  for(int i = 1; i <= n; i++){
    scanf("%s", s);
    for(int j = 1;j <= n; j++)
      arr[i*MAXN+j] = s[j-1];
  }

  initOpenCL();
  executeOpenCL();
  //TODO: change int to char
  for(int i = 1; i <= n; i++){
    for(int j = 1; j <= n; j++){
      putchar((arr[((m)%2)*MAXN*MAXN+i*MAXN+j]==0)?'0':'1');
    }
    puts("");
  }

  clReleaseContext(context);
  clReleaseCommandQueue(commandQueue);
  clReleaseProgram(program);
  clReleaseKernel(kernel);
  //clReleaseMemObject(countBuffer);	
  clReleaseMemObject(arrayBuffer);
  
  return 0;
}
