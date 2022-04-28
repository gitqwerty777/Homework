#include "MPEGDecoder.h"
#include <cstdio>

int main(int argc, char **argv) {
  assert(argc >= 2);
  char *timestr = NULL;
  char *inputFileName = argv[1];
  if (argc > 2)
    timestr = argv[2];
  MPEGDecoder mpegDecoder(inputFileName, timestr);
  mpegDecoder.decode();
}
