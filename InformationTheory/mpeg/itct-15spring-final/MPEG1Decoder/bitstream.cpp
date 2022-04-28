#include "MPEG1Decoder.h"

BitStream::~BitStream()
{
    if (fp != NULL) fclose(fp);
}

void BitStream::open(const char *filename)
{
    fp = fopen(filename, "rb");
    if (fp == NULL) throw "error opening file";
    pos_file = fread(buf, 1, BUFSIZE, fp);
    pos_byte = 0;
    pos_bit = 7;
}

uint32_t BitStream::nextbits(int count, bool pop)
{
    uint32_t ret = 0;
    int left = count;
    int npos_byte = pos_byte, npos_bit = pos_bit;
    if (npos_bit != 7 && left > npos_bit) {
        ret = buf[npos_byte % BUFSIZE] & ((1 << (npos_bit + 1)) - 1);
        left -= npos_bit + 1;
        npos_byte++;
        npos_bit = 7;
    }
    while (left >= 8) {
        if (npos_byte >= pos_file) {
            pos_file += fread(buf + pos_file % BUFSIZE, 1, BUFSIZE >> 1, fp);
            if (npos_byte >= pos_file) throw "end of file reached";
        }
        ret = ret << 8 | buf[npos_byte % BUFSIZE];
        left -= 8;
        npos_byte++;
    }
    if (left) {
        if (npos_byte >= pos_file) {
            pos_file += fread(buf + pos_file % BUFSIZE, 1, BUFSIZE >> 1, fp);
            if (npos_byte >= pos_file) throw "end of file reached";
        }
        ret = ret << left | (buf[npos_byte % BUFSIZE] >> (npos_bit - left + 1) & ((1 << left) - 1));
        npos_bit -= left;
    }
    if (pop) {
        pos_byte = npos_byte;
        pos_bit = npos_bit;
    }
    return ret;
}

void BitStream::align()
{
    if (pos_bit != 7) {
        pos_byte++;
        pos_bit = 7;
    }
}

int BitStream::getpos()
{
    return pos_byte << 3 | (7 - pos_bit);
}

void BitStream::setpos(int newpos)
{
    pos_byte = newpos >> 3;
    pos_bit = 7 - (newpos & 0x7);
    fseek(fp, pos_byte / BUFSIZE * BUFSIZE, SEEK_SET);
    pos_file = pos_byte / BUFSIZE * BUFSIZE + fread(buf, 1, BUFSIZE, fp);
}