#ifndef _BIT_READER_H_
#define _BIT_READER_H_
#include <cstdio>
#include "magic_code.h"
class BitReader {
private:
    FILE *const file;
    byte now;
    byte mask;

    fpos_t prev_pos;
    byte prev;
    byte prev_mask;
public:
    BitReader(FILE *__file);
    byte read();
    int read(int nbits);
    void read(byte *buf, size_t size);
    bool next_bits(const char *const bits, bool keep=true);
    void next_start_code();
    void save();
    void restore();
};

class HuffmanTree {
private:
    struct Node {
        int val;
        bool leaf;
        Node *child[2];
        Node () {
            child[0] = child[1] = nullptr;
            leaf = false;
        }
    } *root;
public:
    HuffmanTree();
    void insertNode(const char *const path, int val);
    int decode(BitReader &bitReader);
};
#endif
