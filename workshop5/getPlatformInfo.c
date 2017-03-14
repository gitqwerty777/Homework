/* header */
#include <stdio.h>
#include <assert.h>
#include <CL/cl.h>
#define MAXB 256
#define MAXPLATFORM 5

void runtest(char* buffer, int length){
  if(buffer[length] != '\0'){
    printf("platform no add \\0\n");
  } else {
    printf("platform yes add \\0\n");
  }
}

int main(int argc, char *argv[])
{
  printf("hello, OpenCL\n");
  cl_platform_id platform_id[MAXPLATFORM];
  cl_uint platform_id_got;
  clGetPlatformIDs(MAXPLATFORM, platform_id, 
		   &platform_id_got);
  printf("%d platform found\n", platform_id_got);
  /* getinfo */
  for (int i = 0; i < platform_id_got; i++) {
    char buffer[MAXB];
    size_t length;
    clGetPlatformInfo(platform_id[i], CL_PLATFORM_NAME, 
		      MAXB, buffer, &length);
    runtest(buffer, length);
    
    buffer[length] = '\0';
    printf("platform name %s\n", buffer);
    clGetPlatformInfo(platform_id[i], CL_PLATFORM_VENDOR, 
		      MAXB, buffer, &length);
    runtest(buffer, length);    
    buffer[length] = '\0';
    printf("platform vendor %s\n", buffer);
    clGetPlatformInfo(platform_id[i], CL_PLATFORM_VERSION, 
		      MAXB, buffer, &length);
    runtest(buffer, length);    
    buffer[length] = '\0';
    printf("OpenCL version %s\n", buffer);
    clGetPlatformInfo(platform_id[i], CL_PLATFORM_PROFILE, 
		      MAXB, buffer, &length);
    runtest(buffer, length);    
    buffer[length] = '\0';
    printf("platform profile %s\n", buffer);
  }
  return 0;
}
/* end */
