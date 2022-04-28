#include <cstdio>
#include <cmath>
#include <cstring>
#include <cassert>
#include <algorithm>
#include "magic_code.h"
#include "bit_reader.h"
#include "video.h"

#define EAT(X) assert(stream.next_bits(X, false));
#define LOG(MSG) puts(MSG);

const char start_code[]           = "000000000000000000000001";
const char picture_start_code[]   = "00000000000000000000000100000000";
const char user_data_start_code[] = "00000000000000000000000110110010";
const char sequence_header_code[] = "00000000000000000000000110110011";
const char sequence_error_code[]  = "00000000000000000000000110110100";
const char extension_start_code[] = "00000000000000000000000110110101";
const char sequence_end_code[]    = "00000000000000000000000110110111";
const char group_start_code[]     = "00000000000000000000000110111000";
const char macroblock_stuffing[]  = "00000001111";
const char macroblock_escape[]    = "00000001000";

/* parse sequance layer */
void VideoDecoder::video_sequence(BitReader &stream) {
    LOG("video sequence");
    stream.next_start_code();
    do {
        sequence_header(stream);
        do {
            group_of_pictures(stream);
        } while(stream.next_bits(group_start_code));
    } while(stream.next_bits(sequence_header_code));
    display(b_buf); // last backward frame
    EAT(sequence_end_code);
}

/* parse sequance header */
void VideoDecoder::sequence_header(BitReader &stream) {
    LOG("sequence header");
    EAT(sequence_header_code);
    h_size = stream.read(12);
    v_size = stream.read(12);
    mb_width = (h_size+15)/16; // /16 & ceil
    per_ratio = stream.read(4);
    picture_rate = stream.read(4);
    bit_rate = stream.read(18);
    EAT("1"); //marker bit
    vbv_buffer_size = stream.read(10);
    const_param_flag = stream.read();
    bool load_intra_quantizer_matrix = stream.read();
    // check flag bit
    if(load_intra_quantizer_matrix) {
        stream.read(intra_quant_matrix, 64);
    }
    bool load_non_intra_quantizer_matrix = stream.read();
    // check flag bit
    if(load_non_intra_quantizer_matrix) {
        stream.read(non_intra_quant_matrix, 64);
    }
    // align byte
    stream.next_start_code();
    if(stream.next_bits(extension_start_code)) {
        EAT(extension_start_code);
        while (!stream.next_bits(start_code)) {
            stream.read(8); // sequence_extension_data
        }
        stream.next_start_code();
    }
    if(stream.next_bits(user_data_start_code)) {
        EAT(user_data_start_code);
        while (!stream.next_bits(start_code)) {
            stream.read(8); // user_data
        }
        stream.next_start_code();
    }
}

/* parse gop layer */
void VideoDecoder::group_of_pictures(BitReader &stream) {
    LOG("group of pictures");
    EAT(group_start_code);
    int time_code = stream.read(25);
    bool closed_gop = stream.read();
    bool broken_link = stream.read();
    assert(!broken_link);
    stream.next_start_code();
    if(stream.next_bits(extension_start_code)) {
        EAT(extension_start_code);
        while (!stream.next_bits(start_code)) {
            stream.read(8); // sequence_extension_data
        }
        stream.next_start_code();
    }
    if(stream.next_bits(user_data_start_code)) {
        EAT(user_data_start_code);
        while (!stream.next_bits(start_code)) {
            stream.read(8); // user_data
        }
        stream.next_start_code();
    }
    do {
        picture(stream);
    } while(stream.next_bits(picture_start_code));
}

/* check if the following bits are slice start code */
inline bool is_slice_start_code(BitReader &stream) {
    stream.save();
    if(!stream.next_bits(start_code, false)) {
        stream.restore();
        return false;
    }
    byte code = stream.read(8);
    stream.restore();
    return 0x01 <= code and code <= 0xAF;
}

/* parse picture layer */
void VideoDecoder::picture(BitReader &stream) {
    LOG("picture");
    EAT(picture_start_code);
    tmp_ref = stream.read(10);
    coding_type = stream.read(3);

    /* 3-Frame Buffers Algorithm - before decode*/
    if(coding_type <= 2) {
        std::swap(f_buf, b_buf);
        display(f_buf);
    }

    vbv_delay = stream.read(16);
    if(coding_type == 2 || coding_type == 3) { // P, B Frame
        // read forward size
        full_pel_forward_vector = stream.read();
        byte forward_f_code = stream.read(3);
        assert(forward_f_code != 0);
        forward_r_size = forward_f_code - 1;
        forward_f = 1 << forward_r_size;
    }
    if(coding_type == 3) { // B frame
        // read backward size
        full_pel_backward_vector = stream.read();
        byte backward_f_code = stream.read(3);
        assert(backward_f_code != 0);
        backward_r_size = backward_f_code - 1;
        backward_f = 1 << backward_r_size;
    }
    while(stream.next_bits("1")) {
        EAT("1"); // extra_bit_picture
        stream.read(8); // extra_info_picture
    }
    EAT("0"); // extra_bit_picture

    stream.next_start_code();
    if(stream.next_bits(extension_start_code)) {
        EAT(extension_start_code);
        while (!stream.next_bits(start_code)) {
            stream.read(8); // sequence_extension_data
        }
        stream.next_start_code();
    }
    if(stream.next_bits(user_data_start_code)) {
        EAT(user_data_start_code);
        while (!stream.next_bits(start_code)) {
            stream.read(8); // user_data
        }
        stream.next_start_code();
    }
    do {
        slice(stream);
    } while(is_slice_start_code(stream));

    /* 3-Frame Buffers Algorithm - after decode*/
    if(coding_type <= 2) {
        std::swap(c_buf, b_buf);
    }
    else {
        display(c_buf);
    }
}

/* parse slice layer */
void VideoDecoder::slice(BitReader &stream) {
    LOG("slice");
    EAT(start_code);
    slice_vert_pos = stream.read(8);
    quant_scale = stream.read(5);
    while(stream.next_bits("1")) {
        EAT("1"); // extra_bit_slice
        stream.read(8); // extra_info_slice
    }
    EAT("0"); // extra_bit_slice

    /* reset previous variables */
    past_intra_addr = -2;
    macroblock_addr = (slice_vert_pos-1)*mb_width-1;
    recon_right_for_prev = recon_down_for_prev = 0;
    recon_right_back_prev = recon_down_back_prev = 0;
    dct_dc_y_past = dct_dc_cb_past = dct_dc_cr_past = 1024;

    do {
        macroblock(stream);
    } while(!stream.next_bits("00000000000000000000000"));
    stream.next_start_code();
}

inline int calc_recon_motion(int f, int motion_code,
        int motion_r, int &recon_prev, bool full_pel_vector) {
    int complement_r = (f==1 || motion_code==0)?(0):(f-1-motion_r);
    int little = motion_code*f, big;
    if(little==0) {
        big = 0;
    } else {
        if(little > 0) {
            little = little - complement_r;
            big = little - 32*f;
        } else {
            little = little + complement_r;
            big = little + 32*f;
        }
    }

    int max_v = 16*f -1;
    int min_v = -16*f;

    int recon;
    int new_v = recon_prev + little;
    if(min_v <= new_v && new_v <= max_v)
        recon = recon_prev + little;
    else
        recon = recon_prev + big;
    recon_prev = recon;
    if(full_pel_vector) recon = recon << 1;
    return recon;
}

/* parse macroblock layer */
void VideoDecoder::macroblock(BitReader &stream) {
    //LOG("macroblock");
    while(stream.next_bits(macroblock_stuffing))
        EAT(macroblock_stuffing);
    int macroblock_addr_increment = 0;
    while(stream.next_bits(macroblock_escape)) {
        // add 33 when meet escape code
        EAT(macroblock_escape);
        macroblock_addr_increment += 33;
    }
    macroblock_addr_increment += ht_macroblock_addr.decode(stream);

    /* handle skipped macroblock */
    for(int i=1; i<macroblock_addr_increment; ++i)
        write_skipped_macroblock(macroblock_addr+i);

    // update macroblcok_addr
    macroblock_addr += macroblock_addr_increment;

    // get coded macroblock type
    if(coding_type == 1)
        macroblock_type = ht_intra_macroblock_type.decode(stream);
    else if(coding_type == 2)
        macroblock_type = ht_p_macroblock_type.decode(stream);
    else if(coding_type == 3)
        macroblock_type = ht_b_macroblock_type.decode(stream);
    else
        assert(false);

    // check quant_scale field flag
    if(macroblock_type & mask_macroblock_quant) {
        quant_scale = stream.read(5);
        assert(quant_scale != 0);
    }

    // check motion forward field flag
    if(macroblock_type & mask_macroblock_motion_f) {
        int motion_h_f_code, motion_h_f_r;
        int motion_v_f_code, motion_v_f_r;

        motion_h_f_code = ht_motion_vector.decode(stream);
        if((forward_f != 1) and
            (motion_h_f_code != 0))
            motion_h_f_r = stream.read(forward_r_size);

        motion_v_f_code = ht_motion_vector.decode(stream);
        if((forward_f != 1) and
            (motion_v_f_code != 0))
            motion_v_f_r = stream.read(forward_r_size);

        recon_right_for =
            calc_recon_motion(forward_f, motion_h_f_code, motion_h_f_r,
                recon_right_for_prev, full_pel_forward_vector);

        recon_down_for =
            calc_recon_motion(forward_f, motion_v_f_code, motion_v_f_r,
                recon_down_for_prev, full_pel_forward_vector);
    } else {
        if(coding_type == 2) {
            // reset recon_for_prev
            recon_down_for_prev = recon_right_for_prev = 0;
            recon_right_for = recon_down_for = 0;
        }
    }

    // check motion backward field flag
    if(macroblock_type & mask_macroblock_motion_b) {
        int motion_h_b_code, motion_h_b_r;
        int motion_v_b_code, motion_v_b_r;

        motion_h_b_code = ht_motion_vector.decode(stream);
        if((backward_f != 1) and
            (motion_h_b_code != 0))
            motion_h_b_r = stream.read(backward_r_size);

        motion_v_b_code = ht_motion_vector.decode(stream);
        if((backward_f != 1) and
            (motion_v_b_code != 0))
            motion_v_b_r = stream.read(backward_r_size);

        recon_right_back =
            calc_recon_motion(backward_f, motion_h_b_code, motion_h_b_r,
                recon_right_back_prev, full_pel_backward_vector);

        recon_down_back =
            calc_recon_motion(backward_f, motion_v_b_code, motion_v_b_r,
                recon_down_back_prev, full_pel_backward_vector);
    }

    // init cbp with all '1'
    byte cbp = 0;
    if(macroblock_type & mask_macroblock_pattern) {
        cbp = ht_coded_block_pattern.decode(stream);
    }
    else if(macroblock_type & mask_macroblock_intra) {
        // intra frame - reset recon_prev
        recon_right_for_prev = recon_down_for_prev = 0;
        recon_right_back_prev = recon_down_back_prev = 0;
        cbp = (1<<6) - 1;
    }

    for(int i=0; i<6; ++i) {
        if(cbp & (1<<(5-i))) {
            block(i, stream);
            recon_idct(i);
        } else {
            for(int j=0; j<8; ++j) for(int k=0; k<8; ++k) block_buf[j][k] = 0;
        }
        if(!(macroblock_type & mask_macroblock_intra)) {
            add_motion_vector(i);
        }
        write_block(i, macroblock_addr);
    }

    // update past_intra_addr
    if(macroblock_type & mask_macroblock_intra)
        past_intra_addr = macroblock_addr;

    if(coding_type == 4)
        EAT("1");
    return;
}

void VideoDecoder::block(int index, BitReader &stream) {
    //LOG("block");
    memset(dct_zz, 0, sizeof(dct_zz));
    int i=0;
    if(macroblock_type & mask_macroblock_intra) {
        // intra block
        int size = 0;
        if(index<4) {
            size = ht_dct_dc_size_luminance.decode(stream);
        }
        else {
            size = ht_dct_dc_size_chrominance.decode(stream);
        }
        if(size == 0) {
            dct_zz[0] = 0;
        }
        else {
            int dct_dc_diff = stream.read(size);
            if(dct_dc_diff & (1<<(size-1))) dct_zz[0] = dct_dc_diff;
            else dct_zz[0] = (-1 << size) | (dct_dc_diff+1);
        }
    }
    else {
        // non-intra block
        int run, level;
        std::tie(run, level) = decode_run_level(stream, true);
        i = run;
        dct_zz[i] = level;
    }
    if(coding_type != 4) {
        // ac coefs
        while(!stream.next_bits("10")) {
            int run, level;
            std::tie(run, level) = decode_run_level(stream);
            i = i+run+1;
            assert(i < 64);
            dct_zz[i] = level;
        }
        EAT("10");
    }
};

inline void idct(double res[8], double mat[8]) {
    double c2 = 2*cos(M_PI/8);
    double c4 = 2*cos(2*M_PI/8);
    double c6 = 2*cos(3*M_PI/8);
    double sq8 = sqrt(8);

    // B1
    double a0 = (1./8*mat[0])*sq8;
    double a1 = (1./8*mat[4])*sq8;
    double a2 = (1./8*mat[2] - 1./8*mat[6])*sq8;
    double a3 = (1./8*mat[2] + 1./8*mat[6])*sq8;
    double a4 = (1./8*mat[5] - 1./8*mat[3])*sq8;
    double temp1 = (1./8*mat[1] + 1./8*mat[7])*sq8;
    double temp2 = (1./8*mat[3] + 1./8*mat[5])*sq8;
    double a5 = temp1 - temp2;
    double a6 = (1./8*mat[1] - 1./8*mat[7])*sq8;
    double a7 = temp1+temp2;

    // M
    double b0 = a0;
    double b1 = a1;
    double b2 = a2*c4;
    double b3 = a3;
    double Q = c2-c6, R = c2+c6, temp4 = c6*(a4+a6);
    double b4 = -Q*a4 - temp4;
    double b5 = a5*c4;
    double b6 = R*a6 - temp4;
    double b7 = a7;

    // A1
    double temp3 = b6 - b7;
    double n0 = temp3 - b5;
    double n1 = b0 - b1;
    double n2 = b2 - b3;
    double n3 = b0 + b1;
    double n4 = temp3;
    double n5 = b4;
    double n6 = b3;
    double n7 = b7;

    // A2
    double m0 = n7;
    double m1 = n0;
    double m2 = n4;
    double m3 = n1 + n2;
    double m4 = n3 + n6;
    double m5 = n1 - n2;
    double m6 = n3 - n6;
    double m7 = n5 - n0;

    // A3
    res[0] = m4 + m0;
    res[1] = m3 + m2;
    res[2] = m5 - m1;
    res[3] = m6 - m7;
    res[4] = m6 + m7;
    res[5] = m5 + m1;
    res[6] = m3 - m2;
    res[7] = m4 - m0;
    return;
}

inline void transpose(double mat[8][8]) {
    for(int i=0; i<7; ++i)
        for(int j=i+1; j<8; ++j)
            std::swap(mat[i][j], mat[j][i]);
    return;
}

inline void idct2d(double mat[8][8]) {
    double row[8][8];
    memcpy(row, mat, sizeof(row));
    for(int i=0; i<8; ++i)
        idct(row[i], mat[i]);
    transpose(row);
    for(int i=0; i<8; ++i)
        idct(mat[i], row[i]);
    transpose(mat);
    return;
}

void VideoDecoder::recon_idct(int index) {
    #define SIGN(x) ((x > 0) - (x < 0))
    // recontruct dct ac component
    for(int m=0; m<8; ++m) {
        for(int n=0; n<8; ++n) {
            int i = scan[m][n];
            int tmp;
            if(macroblock_type & mask_macroblock_intra)
                tmp =
                    (2*dct_zz[i]*quant_scale*intra_quant_matrix[i])/16;
            else
                tmp = (((2*dct_zz[i])+SIGN(dct_zz[i]))*
                    quant_scale*non_intra_quant_matrix[i])/16;
            if((tmp & 1) == 0)
                tmp = tmp - SIGN(tmp);
            if(tmp > 2047) tmp = 2047;
            if(tmp < -2048) tmp = -2048;
            if(dct_zz[i] == 0) tmp = 0;
            block_buf[m][n] = tmp;
        }
    }

    if(macroblock_type & mask_macroblock_intra) {
        // reconstruct dct dc component
        double *dct_dc_past;
        if(index < 4) dct_dc_past = &dct_dc_y_past;
        else if(index == 4) dct_dc_past = &dct_dc_cb_past;
        else dct_dc_past = &dct_dc_cr_past;

        block_buf[0][0] = dct_zz[0]*8;
        if((index == 0 || index > 3) &&
            macroblock_addr-past_intra_addr > 1) {
            // first block
            block_buf[0][0] = 128*8 + block_buf[0][0];
        }
        else {
            // not first block
            block_buf[0][0] = *dct_dc_past + block_buf[0][0];
        }
        *dct_dc_past = block_buf[0][0];
    }
    idct2d(block_buf);
}

template<int W>
inline void add_pel_past(double pel[8][8], double pel_past[][W],
        int recon_right, int recon_down,
        int index, bool half,
        int macroblock_addr, int mb_width) {
        int right, down, right_half, down_half;
    if(index < 4) {
        right = recon_right >> 1;
        down = recon_down >> 1;
        right_half = recon_right - 2*right ;
        down_half = recon_down - 2*down;
    } else {
        right = (recon_right/2) >> 1;
        down = (recon_down/2) >> 1;
        right_half = recon_right/2 - 2*right;
        down_half = recon_down/2 - 2*down;
    }

    double weight = half?0.5:1;

    int mb_row = macroblock_addr/mb_width;
    int mb_col = macroblock_addr%mb_width;
    int top = mb_row*16;
    int left = mb_col*16;
    switch(index) {
        case 0: break;
        case 1: left+=8; break;
        case 2: top+=8; break;
        case 3: left+=8, top+=8; break;
        default: top /= 2, left /= 2;
    }
    if(!right_half && !down_half) {
        for(int i=0; i<8; ++i)
            for(int j=0; j<8; ++j) {
                pel[i][j] +=
                    pel_past[top+i+down][left+j+right]*weight;
            }
    } else if(!right_half && down_half) {
        for(int i=0; i<8; ++i)
            for(int j=0; j<8; ++j) {
                pel[i][j] +=
                    (pel_past[top+i+down][left+j+right] +
                    pel_past[top+i+down+1][left+j+right])/2*weight;
            }
    } else if(right_half && !down_half) {
        for(int i=0; i<8; ++i)
            for(int j=0; j<8; ++j) {
                pel[i][j] +=
                    (pel_past[top+i+down][left+j+right] +
                    pel_past[top+i+down][left+j+right+1])/2*weight;
            }
    } else if(right_half && down_half) {
        for(int i=0; i<8; ++i)
            for(int j=0; j<8; ++j) {
                pel[i][j] +=
                    (pel_past[top+i+down][left+j+right] +
                    pel_past[top+i+down][left+j+right+1] +
                    pel_past[top+i+down+1][left+j+right] +
                    pel_past[top+i+down+1][left+j+right+1])/4*weight;
            }
    }
}

void VideoDecoder::write_skipped_macroblock(int address) {
    if(coding_type == 2) {
        recon_right_for = recon_down_for = 0;
        recon_right_back = recon_down_back = 0;
        recon_right_for_prev = recon_down_for_prev = 0;
        recon_right_back_prev = recon_down_back_prev = 0;
    }

    bool has_f = coding_type == 2 || (macroblock_type & mask_macroblock_motion_f);
    bool has_b = macroblock_type & mask_macroblock_motion_b;
    bool half = has_f && has_b;

    for(int y=0; y<4; ++y) {
        for(int i=0; i<8; ++i) for(int j=0; j<8; ++j) block_buf[i][j] = 0;
        if(has_f)
            add_pel_past(block_buf, f_buf->y,
                recon_right_for, recon_down_for, y, half,
                address, mb_width);
        if(has_b)
            add_pel_past(block_buf, b_buf->y,
                recon_right_back, recon_down_back, y, half,
                address, mb_width);
        write_block(y, address);
    }
    {
        for(int i=0; i<8; ++i) for(int j=0; j<8; ++j) block_buf[i][j] = 0;
        if(has_f)
            add_pel_past(block_buf, f_buf->cb,
                recon_right_for, recon_down_for, 4, half,
                address, mb_width);
        if(has_b)
            add_pel_past(block_buf, b_buf->cb,
                recon_right_back, recon_down_back, 4, half,
                address, mb_width);
        write_block(4, address);
    }
    {
        for(int i=0; i<8; ++i) for(int j=0; j<8; ++j) block_buf[i][j] = 0;
        if(has_f)
            add_pel_past(block_buf, f_buf->cr,
                recon_right_for, recon_down_for, 5, half,
                address, mb_width);
        if(has_b)
            add_pel_past(block_buf, b_buf->cr,
                recon_right_back, recon_down_back, 5, half,
                address, mb_width);
        write_block(5, address);
    }
}

void VideoDecoder::add_motion_vector(int index) {
    bool has_f = coding_type == 2 || (macroblock_type & mask_macroblock_motion_f);
    bool has_b = macroblock_type & mask_macroblock_motion_b;
    bool half = has_f && has_b;
    if(has_f) {
        if(index < 4)
            add_pel_past(block_buf, f_buf->y,
                recon_right_for, recon_down_for, index, half,
                macroblock_addr, mb_width);
        else if(index == 4)
            add_pel_past(block_buf, f_buf->cb,
                recon_right_for, recon_down_for, index, half,
                macroblock_addr, mb_width);
        else
            add_pel_past(block_buf, f_buf->cr,
                recon_right_for, recon_down_for, index, half,
                macroblock_addr, mb_width);
    }
    if(has_b) {
        if(index < 4)
            add_pel_past(block_buf, b_buf->y,
                recon_right_back, recon_down_back, index, half,
                macroblock_addr, mb_width);
        else if(index == 4)
            add_pel_past(block_buf, b_buf->cb,
                recon_right_back, recon_down_back, index, half,
                macroblock_addr, mb_width);
        else
            add_pel_past(block_buf, b_buf->cr,
                recon_right_back, recon_down_back, index, half,
                macroblock_addr, mb_width);
    }
}

void VideoDecoder::write_block(int index, int addr) {
    // calculate row, column
    int mb_row = addr/mb_width;
    int mb_col = addr%mb_width;
    if(index <=3) {
        // Y
        int top = mb_row*16;
        int left = mb_col*16;
        switch(index) {
            case 0: break;
            case 1: left+=8; break;
            case 2: top+=8; break;
            case 3: left+=8, top+=8; break;
            default: assert(false);
        }
        for(int i=0; i<8; ++i)
            for(int j=0; j<8; ++j)
                c_buf->y[i+top][j+left] =
                    std::max(0., std::min(255., block_buf[i][j]));
    }
    else {
        int top = mb_row*8;
        int left = mb_col*8;
        if(index == 4) {
            // Cb
            for(int i=0; i<8; ++i)
                for(int j=0; j<8; ++j)
                    c_buf->cb[i+top][j+left] =
                        std::max(0., std::min(255., block_buf[i][j]));
        } else {
            // Cr
            for(int i=0; i<8; ++i)
                for(int j=0; j<8; ++j)
                    c_buf->cr[i+top][j+left] =
                        std::max(0., std::min(255., block_buf[i][j]));
        }
    }
}
