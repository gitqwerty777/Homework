#ifndef _MCU_DECODE_H_
#define _MCU_DECODE_H_
#include "huffman_decode.h"
class MCUDecoder {
private:
    HuffmanTree *DCHuffman, *ACHuffman;
    int qTable[64];
    int diff;
    void decodeHuffman(int res[64], BitReader &bitReader);
    void unQuantize(int zigzag[64]);
    void unZigZag(double res[8][8], int zigzag[64]);
    void idct(double res[8], double input[8]);
    void transpose(double mat[8][8]);
    void idct2d(double res[8][8]);
    void add128(double mat[8][8]);
public:
    MCUDecoder(HuffmanTree *DCHuffman, HuffmanTree *ACHuffman, int *qTable);
    void decode(double block[8][8], BitReader &bitReader);
    void resetDiff();
};
#endif
