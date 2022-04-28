#ifndef _JFIF_HEADER_H_
#define _JFIF_HEADER_H_
#define CODE_SOI  0xFFD8
#define CODE_DQT  0xFFDB
#define CODE_SOF0 0xFFC0
#define CODE_DHT  0xFFC4
#define CODE_DRI  0xFFDD
#define CODE_SOS  0xFFDA
#define CODE_EOI  0xFFD9
typedef unsigned char byte;
#define B2L(x) (((int)(*((byte*)(x))))<<8 | (*(((byte*)(x))+1)))

#pragma pack(push,1)
struct APP0 {
    byte length[2];
    byte id[5]; //"JFIF0"
    byte ver[2];
    byte d_unit;
    byte dx[2];
    byte dy[2];
    byte thumb_h;
    byte thumb_v;
    byte thumb[0];
};

struct QT {
    byte prec_id;
    byte table[0];
};

struct DQT {
    byte length[2];
    byte tables[0];
};

struct CInfo {
    byte id;
    byte h_v_coef;
    byte qt_id;
};

struct SOF0 {
    byte length[2];
    byte prec;
    byte height[2];
    byte width[2];
    byte color;
    CInfo cInfo[0];
};

struct HT {
    byte type_id;
    byte count[16];
    byte table[0];
};

struct DHT {
    byte length[2];
    byte tables[0];
};

struct DRI {
    byte length[2];
    byte gap[2];
};

struct CHT {
    byte id;
    byte ht_id;
};

struct SOS {
    byte length[2];
    byte color;
    byte tables[0];
};
#pragma pack(pop)
#endif
