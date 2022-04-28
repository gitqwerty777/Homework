#include <cstdio>
#include <bit_buf.h>
#include <start_codes.h>
#include <cassert>
#ifndef GROUP_OF_PICTURES_H
#define GROUP_OF_PICTURES_H
struct group_of_pictures{
	bit_buf* bb;
	uint32_t group_start_code;
	uint32_t time_code;
	uint8_t closed_gop;
	uint8_t broken_link;
	group_of_pictures(bit_buf* _bb):bb(_bb){}
	void read(){
		group_start_code = bb->get(32);
		assert(group_start_code == GROUP_START_CODE);
		time_code = bb->get(25);
		closed_gop = bb->get(1);
		broken_link = bb->get(1);
		bb->next_start_code();
		if(bb->nextbits() == EXTENSION_START_CODE){
			while(bb->nextbits() != 0x000001){
				bb->get(8);
			}
			bb->next_start_code();
		}
		if(bb->nextbits() == USER_DATA_START_CODE){
			while(bb->nextbits() != 0x00001){
				bb->get(8);
			}
			bb->next_start_code();
		}
	}
};
#endif
