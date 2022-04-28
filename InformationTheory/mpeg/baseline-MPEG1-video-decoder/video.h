#ifndef _VIDEO_H_
#define _VIDEO_H_
#include <tuple>
#include <vector>
#include "bit_reader.h"
//class CImgDisplay;
#include "CImg.h"
using namespace cimg_library;
extern const int scan[8][8];
extern const byte mask_macroblock_quant;
extern const byte mask_macroblock_motion_f;
extern const byte mask_macroblock_motion_b;
extern const byte mask_macroblock_pattern;
extern const byte mask_macroblock_intra;
struct YCbCrBuffer {
    double y[768][576];
    double cb[384][288];
    double cr[384][288];
};
class VideoDecoder {
private:
    /* display */
    CImgDisplay main_disp;

    /* huffman trees */
    HuffmanTree ht_macroblock_addr;
    HuffmanTree ht_coded_block_pattern;
    HuffmanTree ht_motion_vector;
    HuffmanTree ht_dct_dc_size_luminance;
    HuffmanTree ht_dct_dc_size_chrominance;
    HuffmanTree ht_intra_macroblock_type;
    HuffmanTree ht_p_macroblock_type;
    HuffmanTree ht_b_macroblock_type;
    HuffmanTree ht_run_level_ind;
    std::vector<int> run_list, level_list;

    /* sequence header */
    int h_size, v_size, mb_width;
    byte per_ratio, picture_rate;
    int bit_rate;
    int vbv_buffer_size;
    bool const_param_flag;
    byte *intra_quant_matrix;
    byte *non_intra_quant_matrix;

    /* picture */
    int tmp_ref;
    byte coding_type;
    int vbv_delay;
    bool full_pel_forward_vector;
    byte forward_r_size;
    byte forward_f;
    bool full_pel_backward_vector;
    byte backward_r_size;
    byte backward_f;

    /* slice */
    byte slice_vert_pos;
    byte quant_scale;

    /* macroblock */
    byte macroblock_type;

    /* buffer */
    int dct_zz[64];
    double block_buf[8][8];
    YCbCrBuffer *b_buf, *c_buf, *f_buf;

    /* now */
    int macroblock_addr;
    int recon_right_for, recon_down_for;
    int recon_right_back, recon_down_back;

    /* past */
    int past_intra_addr;
    double dct_dc_y_past, dct_dc_cb_past, dct_dc_cr_past;
    int recon_right_for_prev, recon_down_for_prev;
    int recon_right_back_prev, recon_down_back_prev;

    std::tuple<int, int> decode_run_level(BitReader &stream, bool first=false);
    
public:
    VideoDecoder();
    void video_sequence(BitReader &stream);
    void sequence_header(BitReader &stream);
    void group_of_pictures(BitReader &stream);
    void picture(BitReader &stream);
    void slice(BitReader &stream);
    void macroblock(BitReader &stream);
    void block(int index, BitReader &stream);

    void write_skipped_macroblock(int address);
    void add_motion_vector(int index);
    void recon_idct(int index);
    void write_block(int index, int addr);
    void display(YCbCrBuffer *buf);
};
#endif
