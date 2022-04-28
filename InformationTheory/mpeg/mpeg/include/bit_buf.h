#include <cstdio>
#include <cstdint>
#include <cassert>
#ifndef BIT_BUF_H
#define BIT_BUF_H
struct bit_buf{
	uint64_t buffer;
	uint16_t now;
	uint8_t byte_buf;
	uint64_t one;
	FILE* file;
	bit_buf(FILE* _file)
		:file(_file){
		one = 1;
		now = 0;
		buffer = 0;
		for(int i=0;i<5;i++){
			fread(&byte_buf,1,1,file);
			buffer = (buffer<<8) | byte_buf;
		}
		//printf("%llu\n",buffer);
	}
	void next_start_code(){
		byte_aligned();
		while(nextbits(24) != 0x000001){
			get(8);
		}
	};
	void byte_aligned(){
		if(now !=0){
			get(8-now);
		}	
		//puts("byte align!!");
	};
	uint32_t nextbits(uint8_t size = 32){
		//printf("%u\n",size);
		assert(size>0);
		//assert(size<=32);
		assert(now < 8);
		uint8_t s = 40-now-size;
		assert(s >= 0 && s < 40);
		uint32_t res = (buffer >> s) & ((1ull<<size)-1);
		/*
		printf("res: %x, now: %hu,buffer: %llx\n",res,now,buffer);
		printf("size: %hhu\n",size);
		*/
		return res;
	};
	uint32_t get(uint8_t size){
		uint32_t res = nextbits(size);
		now += size;
		assert(now < 40);
		assert(size <= 32);
		while(now >= 8){
			fread(&byte_buf,1,1,file);
			buffer = (buffer<<8) | byte_buf;
			now -= 8;
		}
		return res;
	}
};
#endif
