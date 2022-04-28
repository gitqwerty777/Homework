#include <cstdio>
#include <bit_buf.h>
#include <start_codes.h>
#include <picture.h>
#ifndef SEQUENCE_HEADER_H
#define SEQUENCE_HEADER_H
struct sequence_header{
	bit_buf* bb;
	uint32_t sequence_header_code;
	uint16_t horizontal_size;
	uint16_t vertical_size;
	uint8_t pel_aspect_ratio;
	uint8_t picture_rate;
	uint32_t bit_rate;
	uint8_t marker_bit;
	uint16_t vbv_buffer_size;
	uint8_t constrained_parameter_flag;
	uint8_t load_intra_quantizer_matrix;
	uint8_t load_non_intra_quantizer_matrix;
	picture* picture_ref0;
	picture* picture_ref1;
	int pre_macroblock_address;
	int past_intra_address;
	int dct_dc_y_past;
	int dct_dc_cb_past;
	int dct_dc_cr_past;
	int mb_width, mb_height;
	int mb_row, mb_column;
	int recon_right_for_pre, recon_down_for_pre;
	int recon_right_for, recon_down_for;
	int recon_right_back, recon_down_back;
	int recon_right_back_pre, recon_down_back_pre;
	int intra_quantizer_matrix[8][8] = {
		{ 8,16,19,22,26,27,29,34},
		{16,16,22,24,27,29,34,37},
		{19,22,26,27,29,34,34,38},
		{22,22,26,27,29,34,37,40},
		{22,26,27,29,32,35,40,48},
		{26,27,29,32,35,40,48,58},
		{26,27,29,34,38,46,56,69},
		{27,29,35,38,46,56,69,83}
	};
	int non_intra_quantizer_matrix[8][8] = {
		{16,16,16,16,16,16,16,16},
		{16,16,16,16,16,16,16,16},
		{16,16,16,16,16,16,16,16},
		{16,16,16,16,16,16,16,16},
		{16,16,16,16,16,16,16,16},
		{16,16,16,16,16,16,16,16},
		{16,16,16,16,16,16,16,16},
		{16,16,16,16,16,16,16,16}
	};
	sequence_header(bit_buf* _bb):bb(_bb){}
	int counter = 0;
	void read(){
		if(counter ==0){
			picture_ref0 = nullptr;
			picture_ref1 = nullptr;
			counter++;
		}
		sequence_header_code = bb->get(32);
		assert(sequence_header_code == SEQUENCE_HEADER_CODE);
		horizontal_size = bb->get(12);
		vertical_size = bb->get(12);
		mb_width = (horizontal_size+15)/16;
		mb_height = (vertical_size+15)/16;
		pel_aspect_ratio = bb->get(4);
		picture_rate = bb->get(4);
		bit_rate = bb->get(18);
		marker_bit = bb->get(1);
		vbv_buffer_size = bb->get(10);
		constrained_parameter_flag = bb->get(1);
		load_intra_quantizer_matrix = bb->get(1);
		if(load_intra_quantizer_matrix){
			for(int i=0;i<8;i++)
				for(int j=0;j<8;j++)
					intra_quantizer_matrix[i][j] = bb->get(8);
		}
		if(load_non_intra_quantizer_matrix){
			for(int i=0;i<8;i++)
				for(int j=0;j<8;j++)
					non_intra_quantizer_matrix[i][j] = bb->get(8);
		}
		bb->next_start_code();
		if(bb->nextbits() == EXTENSION_START_CODE) {
			bb->get(32);
			while( bb->nextbits() != 0x000001){
				bb->get(8);
			}
			bb->next_start_code();
		}
		if(bb->nextbits() == USER_DATA_START_CODE) {
			bb->get(32);
			while( bb->nextbits() != 0x000001){
				bb->get(8);
			}
			bb->next_start_code();
		}

		puts("read sequence_header done!!!");
	}
};
#endif
