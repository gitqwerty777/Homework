#include <cstdio>
#include <bit_buf.h>
#include <start_codes.h>
#include <cassert>

#ifndef SLICE_H
#define SLICE_H
bool slice_start_code_check(uint32_t code){
	return  code<=0x1af && code>0x100;
}
struct slice{
	bit_buf* bb;
	sequence_header* sh;
	uint32_t slice_start_code;
	uint8_t quantizer_scale;
	uint8_t extra_bit_slice;
	int slice_vertical_position;
	slice(bit_buf* _bb,sequence_header* _sh):bb(_bb),sh(_sh){}
	void read(){
		sh->dct_dc_y_past = sh->dct_dc_cb_past = sh->dct_dc_cr_past = 1024;
		sh->recon_right_for_pre = sh->recon_down_for_pre = 0;
		sh->recon_right_back_pre = sh->recon_down_back_pre = 0;
		sh->past_intra_address = -2;
		slice_start_code = bb->get(32);
		assert(slice_start_code_check(slice_start_code));
		slice_vertical_position = slice_start_code & 0x000000ff;
		sh->pre_macroblock_address=(slice_vertical_position-1)*(sh->mb_width)-1;
		quantizer_scale = bb->get(5);
		while( bb->nextbits() == 1){
			extra_bit_slice = bb->get(1);
			assert(extra_bit_slice == 1);
			bb->get(8);
		}
		extra_bit_slice = bb->get(1);
		assert(extra_bit_slice == 0);

	}
};
#endif
