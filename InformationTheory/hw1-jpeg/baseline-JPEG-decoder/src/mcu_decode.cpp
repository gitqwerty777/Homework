#include <cstring>
#include <cmath>
#include <algorithm>
#include "mcu_decode.h"
void MCUDecoder::decodeHuffman(int res[64], BitReader &bitReader) {
    memset(res, 0, 64*sizeof(int));
    int dc_digit = DCHuffman->decode(bitReader);
    int dc = bitReader.read(dc_digit);
    if(!(dc&(1<<(dc_digit-1))))
        dc = -((~dc)&((1<<dc_digit)-1));
    res[0] = diff+dc;
    diff = res[0];

    int prev = 0;
    while(prev != 63) {
        byte ac_digit = ACHuffman->decode(bitReader);
        if(ac_digit == 0x00) break; //EOB
        int run = ac_digit>>4;
        ac_digit &= 0x0F;
        int ac = bitReader.read(ac_digit);
        if(!(ac&(1<<(ac_digit-1))))
            ac = -((~ac)&((1<<ac_digit)-1));
        res[prev+run+1] = ac;
        prev += run+1;
    }
    return;
}

void MCUDecoder::unQuantize(int zigzag[64]) {
    for(int i=0; i<64; ++i) zigzag[i] *= qTable[i];
    return;
}

void MCUDecoder::unZigZag(double buf[8][8], int zigzag[64]) {
    int ok[8][8] = {0};
    for(int i=0, dir=1, ny=0, nx=0; i<64; ++i) {
        ok[ny][nx] = i;
        buf[ny][nx] = zigzag[i];
        if(dir==1 && (ny==0 || nx==7)) {
            if(ny == 0) nx += 1;
            else ny += 1;
            dir = -dir;
        } else if(dir==-1 && (nx==0 || ny==7)) {
            if(ny == 7) nx += 1;
            else ny += 1;
            dir = -dir;
        } else {
            ny -= dir;
            nx += dir;
        }
    }
    return;
}

void MCUDecoder::idct(double res[8], double mat[8]) {
    for(int i=0; i<8; ++i) {
        res[i] = mat[0]/sqrt(2);
        for(int j=1; j<8; ++j)
            res[i] += mat[j]*cos((2*i+1)*j*M_PI/16);
        res[i] /= 2;
    }
    return;
}

void MCUDecoder::transpose(double mat[8][8]) {
    for(int i=0; i<7; ++i)
        for(int j=i+1; j<8; ++j)
            std::swap(mat[i][j], mat[j][i]);
    return;
}

void MCUDecoder::idct2d(double mat[8][8]) {
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

void MCUDecoder::add128(double mat[8][8]) {
    for(int i=0; i<8; ++i)
        for(int j=i; j<8; ++j)
            mat[i][j] += 128;
}

MCUDecoder::MCUDecoder(HuffmanTree *__DCHuffman, HuffmanTree *__ACHuffman, int *__qTable) {
    DCHuffman = __DCHuffman;
    ACHuffman = __ACHuffman;
    memcpy(qTable, __qTable, sizeof(qTable));
}

void MCUDecoder::decode(double block[8][8], BitReader &bitReader) {
    int zigzag[64];
    decodeHuffman(zigzag, bitReader);
    unQuantize(zigzag);
    unZigZag(block, zigzag);
    idct2d(block);
    fprintf(stderr, "after idct2d:\n");
    for(int i=0; i<8; ++i){
        for(int j=0; j<8; ++j)
            fprintf(stderr, "%lf ", block[i][j]);
        fprintf(stderr, "\n");
    }
    return;
}

void MCUDecoder::resetDiff() {
    diff = 0;
    return;
}
