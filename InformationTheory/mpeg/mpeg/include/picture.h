#include <cstdio>
#include <bit_buf.h>
#include <start_codes.h>
#include <cassert>
#include <vector>
#include <util.h>
using namespace std;
#ifndef PICTURE_H
#define PICTURE_H
struct picture{
	bit_buf* bb;
	int width, height;
	uint32_t picture_start_code;
	uint16_t temporal_reference;
	uint8_t picture_coding_type;
	uint16_t vbv_delay;
	uint8_t full_pel_forward_vector;
	uint8_t full_pel_backward_vector;
	int forward_f_code, forward_r_size, forward_f;
	int backward_f_code, backward_r_size, backward_f;
	int past_intra_address;
	vector<vector<int> > img[3];
	picture(bit_buf* _bb, int _w, int _h)
		:bb(_bb), width(_w), height(_h){}
	picture(vector<vector<int>> _img[3]){
		for(int i=0;i<3;i++) img[i] = _img[i];
	}
	
	picture(const picture& pin){
		for(int i=0;i<3;i++){
			img[i] = pin.img[i];
		}
	}
	void final_picture(int real_width,int real_height){
		for(int i = real_height-1;i>=0;i--){
			for(int j=real_width-1;j>=0;j--){
				img[1][i][j] = img[1][i/2][j/2];
				img[2][i][j] = img[2][i/2][j/2];
				ToRGB(img[0][i][j],img[1][i][j],img[2][i][j]);	
			}
		}
		width = real_width;
		height = real_height;
	}
	void read(){
		for(int i=0;i<3;i++){
			img[i].resize(height);
			for(int j=0;j<height;j++){
				img[i][j].resize(width,0); 
			}
		}
		//printf("img size:%lu %lu\n",img[0][0].size(),img[0].size());
		picture_start_code = bb->get(32);
		assert(picture_start_code == PICTURE_START_CODE);
		temporal_reference = bb->get(10);
		picture_coding_type = bb->get(3);
		vbv_delay = bb->get(16);
		if(picture_coding_type == 2 || picture_coding_type == 3){
			full_pel_forward_vector = bb->get(1);
			forward_f_code = bb->get(3);
			forward_r_size = forward_f_code - 1;
			forward_f = 1 << forward_r_size;
		}
		if(picture_coding_type == 3){
			full_pel_backward_vector = bb->get(1);
			backward_f_code = bb->get(3);
			backward_r_size = backward_f_code - 1;
			backward_f = 1<< backward_r_size;
		}
		uint8_t extra_bit_picture = bb->get(1);
		assert(extra_bit_picture == 0);
		bb->next_start_code();
		if(bb->nextbits() == EXTENSION_START_CODE){
			bb->get(32);
			while(bb->nextbits()!=0x000001){
				bb->get(8);
			}
			bb->next_start_code();
		}
		if(bb->nextbits() == USER_DATA_START_CODE){
			bb->get(32);
			while(bb->nextbits()!=0x000001){
				bb->get(8);	
			}
			bb->next_start_code();
		}
	}
};
#endif
