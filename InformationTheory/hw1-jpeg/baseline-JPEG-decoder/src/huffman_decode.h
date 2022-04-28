#ifndef _HUFFMAN_DECODE_H_
#define _HUFFMAN_DECODE_H_
#include "jfif_header.h"
#include <cstdio>
class BitReader {
private:
    FILE *file;
    byte now;
    byte mask;
public:
    BitReader(FILE *__file);
    byte read();
    int read(int nbits);
};

class HuffmanTree {
private:
    struct Node {
        int ind;
        bool leaf;
        Node *child[2];
        Node () {
            child[0] = child[1] = 0;
            leaf = false;
        }
    } *root;
    byte value[256];
    void insertNode(int path, int ind);
public:
    HuffmanTree(byte *count, int size, byte *__value);
    byte decode(BitReader &bitReader);
};
#endif
