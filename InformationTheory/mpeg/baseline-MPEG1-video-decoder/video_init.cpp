#include <cassert>
#include "video.h"

#define EAT(X) assert(stream.next_bits(X, false));

void read_table(HuffmanTree &ht, const char *filename) {
    FILE *file;
    file = fopen(filename, "r");
    assert(file != nullptr);
    char code[32];
    int val;
    while(fscanf(file, "%s %d", code, &val) != EOF) {
        ht.insertNode(code, val); // insert to huffman tree
    }
    fclose(file);
    return;
}

const byte mask_macroblock_quant = 0x10;
const byte mask_macroblock_motion_f = 0x08;
const byte mask_macroblock_motion_b = 0x04;
const byte mask_macroblock_pattern = 0x02;
const byte mask_macroblock_intra = 0x01;
void read_macroblock_type_table(HuffmanTree &ht, const char *filename) {
    FILE *file;
    file = fopen(filename, "r");
    assert(file != nullptr);
    char code[32];
    int tmp;
    while(fscanf(file, "%s", code) != EOF) {
        int val = 0;
        for(int i=0; i<5; ++i) {
            fscanf(file, "%d", &tmp);
            val = (val<<1) + tmp;
        }
        ht.insertNode(code, val); // insert to huffman tree
    }
    fclose(file);
    return;
}

void read_run_level_table(HuffmanTree &ht, std::vector<int> &run_list, std::vector<int> &level_list, const char *filename) {
    run_list.resize(128);
    level_list.resize(128);
    FILE *file;
    file = fopen(filename, "r");
    assert(file != nullptr);
    char code[32];
    int i=0, run, level;
    while(fscanf(file, "%s %d %d", code, &run, &level) != EOF) {
        run_list[i] = run;
        level_list[i] = level;
        ht.insertNode(code, i++); // insert to huffman tree
    }
    fclose(file);
    return;
}

std::tuple<int, int> VideoDecoder::decode_run_level(BitReader &stream, bool first) {
    int run, level;
    if(stream.next_bits("000001")) { // escape - fixed length
        EAT("000001");
        run = stream.read(6);
        int tmp = stream.read(8);
        if(tmp == 0x00) { // >=128
            level = stream.read(8);
        }
        else if(tmp == 0x80) { // <= -128
            level = stream.read(8)-256;
        }
        else { // otherwise
            level = (char)tmp;
        }
    }
    else {
        int ind = ht_run_level_ind.decode(stream);
        run = run_list[ind];
        level = level_list[ind];
        if(!first && run == 0 && level == 1) {
            EAT("1") // spec NOTE2 and NOTE3
        }
        bool s = stream.read();
        if(s) level = - level;
    }
    return std::make_tuple(run, level);
}
const int scan[8][8] = {
    { 0,  1,  5,  6, 14, 15, 27, 28},
    { 2,  4,  7, 13, 16, 26, 29, 42},
    { 3,  8, 12, 17, 25, 30, 41, 43},
    { 9, 11, 18, 24, 31, 40, 44, 53},
    {10, 19, 23, 32, 39, 45, 52, 54},
    {20, 22, 33, 38, 46, 51, 55, 60},
    {21, 34, 37, 47, 50, 56, 59, 61},
    {35, 36, 48, 49, 57, 58, 62, 63}
};
const int default_intra_quant_matrix[8][8] = {
    { 8, 16, 19, 22, 26, 27, 29, 34},
    {16, 16, 22, 24, 27, 29, 34, 37},
    {19, 22, 26, 27, 29, 34, 34, 38},
    {22, 22, 26, 27, 29, 34, 37, 40},
    {22, 26, 27, 29, 32, 35, 40, 48},
    {26, 27, 29, 32, 35, 40, 48, 58},
    {26, 27, 29, 34, 38, 46, 56, 69},
    {27, 29, 35, 38, 46, 56, 69, 83},
};
const int default_non_intra_quant_matrix[8][8] = {
    {16, 16, 16, 16, 16, 16, 16, 16},
    {16, 16, 16, 16, 16, 16, 16, 16},
    {16, 16, 16, 16, 16, 16, 16, 16},
    {16, 16, 16, 16, 16, 16, 16, 16},
    {16, 16, 16, 16, 16, 16, 16, 16},
    {16, 16, 16, 16, 16, 16, 16, 16},
    {16, 16, 16, 16, 16, 16, 16, 16},
    {16, 16, 16, 16, 16, 16, 16, 16},
};

VideoDecoder::VideoDecoder() {
    b_buf = (YCbCrBuffer*)malloc(sizeof(YCbCrBuffer));
    c_buf = (YCbCrBuffer*)malloc(sizeof(YCbCrBuffer));
    f_buf = (YCbCrBuffer*)malloc(sizeof(YCbCrBuffer));
    intra_quant_matrix = (byte*)malloc(64);
    non_intra_quant_matrix = (byte*)malloc(64);
    for(int i=0; i<8; ++i)
        for(int j=0; j<8; ++j) {
            intra_quant_matrix[scan[i][j]] = default_intra_quant_matrix[i][j];
            non_intra_quant_matrix[scan[i][j]] = default_non_intra_quant_matrix[i][j];
        }
    /* load huffman tables */
    read_table(ht_macroblock_addr, "huffman_tables/macroblock_addr.txt");
    read_table(ht_coded_block_pattern, "huffman_tables/coded_block_pattern.txt");
    read_table(ht_motion_vector, "huffman_tables/motion_vector.txt");
    read_table(ht_dct_dc_size_luminance, "huffman_tables/dct_dc_size_luminance.txt");
    read_table(ht_dct_dc_size_chrominance, "huffman_tables/dct_dc_size_chrominance.txt");
    read_macroblock_type_table(ht_intra_macroblock_type, "huffman_tables/intra_macroblock_type.txt");
    read_macroblock_type_table(ht_p_macroblock_type, "huffman_tables/p_macroblock_type.txt");
    read_macroblock_type_table(ht_b_macroblock_type, "huffman_tables/b_macroblock_type.txt");
    read_run_level_table(ht_run_level_ind, run_list, level_list, "huffman_tables/run_level.txt");
}
