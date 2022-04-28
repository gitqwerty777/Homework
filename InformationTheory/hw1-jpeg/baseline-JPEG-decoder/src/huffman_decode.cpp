#include <cstring>
#include "huffman_decode.h"

BitReader::BitReader(FILE *__file) {
    file = __file;
    mask = 0;
}

byte BitReader::read() {
    byte res;
    if(mask == 0) {
        fread(&now, 1, 1, file); mask = 0x80;
        if(now == 0xFF) {
            fread(&now, 1, 1, file);
            if(now == 0x00) now = 0xFF;
        }
    }
    res = (now&mask)!=0;
    mask >>= 1;
    return res;
}

int BitReader::read(int nbits) {
    int res = 0;
    for(int i=0; i<nbits; ++i)
        res = (res<<1)+read();
    return res;
}

void HuffmanTree::insertNode(int path, int ind) {
    int now = 1<<30;
    Node *ptr = root;
    //printf("%d: ", ind);
    while((now&path) == 0) now >>= 1;
    now >>= 1;
    while(now > 0) {
        int bit = (now&path)!=0;
        //printf("%d", bit);
        if(ptr->child[bit] == nullptr) {
            Node *newNode = new Node();
            ptr->child[bit] = newNode;
        }
        ptr = ptr->child[bit];
        now >>= 1;
    }
    //puts("");
    ptr->ind = ind;
    ptr->leaf = true;
    return;
}

HuffmanTree::HuffmanTree(byte *count, int size, byte *__value) {
    memcpy(value, __value, size);
    root = new Node();
    int ind = 0, pre_digit = 0, path = 0;
    for(int i=0; i<16; ++i) {
        for(int j=0; j<count[i]; ++j) {
            path += 1;
            while(pre_digit < (i+1)) {
                path <<= 1;
                pre_digit += 1;
            }
            insertNode(path, ind++);
        }
    }
}

byte HuffmanTree::decode(BitReader &bitReader) {
    Node *now = root;
    while(!now->leaf) {
        now = now->child[bitReader.read()];
    }
    return value[now->ind];
}
