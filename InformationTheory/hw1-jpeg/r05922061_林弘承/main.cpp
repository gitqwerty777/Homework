#include <cstdio>
#include "JPEGDecoder.h"

int main(int argc, char** argv){
  assert(argc >= 3);
  char* inputFileName = argv[1];
  char* outputFileName = argv[2];
  bool isShow = false;
  if(argc > 3)
      isShow = true;
  JPEGDecoder jpegDecoder(inputFileName, outputFileName, isShow);
  jpegDecoder.decode();
}
