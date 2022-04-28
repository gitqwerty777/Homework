CXXFLAGS=-pipe -std=c++11 -Wall -Wextra -Wpedantic -march=native -g3 -O3
LDFLAGS=-lm -lpthread -lX11
CXX=g++

# Environment
#   Ubuntu 16.04 64bits
#   g++ (Ubuntu 5.4.0-6ubuntu1~16.04.4) 5.4.0 20160609

# Reference
# https://github.com/Tenyoku8478/baseline-MPEG1-video-decoder

# What I done
# real-time IPB MPEG-1 video decodable, which fit the basic requirements
# use CImg as display library

MPEGDecoder: MPEGDecoder.o main.o bitReader.o
	g++ $(CXXFLAGS) MPEGDecoder.o main.o bitReader.o -o MPEGDecoder $(LDFLAGS)
main.o: main.cpp MPEGDecoder.h bitReader.h
MPEGDecoder.o: MPEGDecoder.h bitReader.h
bitReader.o: bitReader.h

# default arguments
ARGS?=MPEG/I_ONLY.M1V

run:
	./MPEGDecoder ${ARGS}
run2:
	./MPEGDecoder MPEG/IP_ONLY.M1V
run3:
	./MPEGDecoder MPEG/IPB_ALL.M1V
debug:
	gdb --args ./MPEGDecoder MPEG/IPB_ALL.M1V
clean:
	rm -f MPEGDecoder *.o
