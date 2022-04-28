#include <cstdlib>
#include <algorithm>
#include "jpg_decode.h"
#include "huffman_decode.h"
#include "mcu_decode.h"

#define READ_CODE(code, file) {fread(&code, 2, 1, file); code = B2L(&code);}

inline void JPGDecoder::read_segment() {
    fread(buf, 2, 1, file);
    fread(buf+2, B2L(buf)-2, 1, file);
    return;
}

bool JPGDecoder::parseDQT(DQT *dqt) {
    puts("parsing DQT...");
    byte *ptr = dqt->tables;
    int length = B2L(dqt->length);
    while(ptr < ((byte*)dqt)+length) {
        QT *qt = (QT*)ptr;
        int prec = qt->prec_id>>4;
        int id = qt->prec_id&0x0F;
        byte *table = qt->table;
        if(prec==0) {
            for(int i=0; i<64; ++i)
                qtable[id][i] = table[i];
        }
        else if(prec==1) {
            for(int i=0; i<64; ++i) {
                qtable[id][i] = B2L(table+i*2);
            }
        }
        ptr = table+64*(1+prec);
    }
    return true;
}

bool JPGDecoder::parseDHT(DHT *dht) {
    puts("parsing DHT...");
    byte *ptr = dht->tables;
    int length = B2L(dht->length);
    while(ptr < ((byte*)dht)+length) {
        HT *ht = (HT*)ptr;
        byte *count = ht->count;
        int size = 0;
        for(int i=0; i<16; ++i) size += count[i];
        int type = ht->type_id>>4, id = ht->type_id&0x0F;
        if(type == 0)
            dc_htree[id] = new HuffmanTree(count, size, ht->table);
        else
            ac_htree[id] = new HuffmanTree(count, size, ht->table);
        ptr = ht->table+size;
    }
    return true;
}

bool JPGDecoder::parseSOF0(SOF0 *sof0) {
    puts("parsing SOF0...");
    height = B2L(sof0->height);
    width = B2L(sof0->width);
    result = (byte*)malloc(height*width*3);
    color_ch_cnt = sof0->color;
    mcu_size_y = mcu_size_x = 0;
    for(int i=0; i<color_ch_cnt; ++i) {
        mcu_ch_cnt_y[i] = sof0->cInfo[i].h_v_coef&0x0F;
        mcu_ch_cnt_x[i] = sof0->cInfo[i].h_v_coef>>4;
        color_qtable_id[i] = sof0->cInfo[i].qt_id;
        mcu_size_y = std::max(mcu_size_y, mcu_ch_cnt_y[i]);
        mcu_size_x = std::max(mcu_size_x, mcu_ch_cnt_x[i]);
    }
    mcu_size_y *= 8;
    mcu_size_x *= 8;
    return true;
}

bool JPGDecoder::parseDRI(DRI *dri) {
    puts("parsing DRI...");
    reset_interval = B2L(dri->gap);
    return true;
}

bool JPGDecoder::parseSOS(SOS *sos) {
    puts("parsing SOS...");
    CHT *cht = (CHT*)sos->tables;
    for(int i=0; i<3; ++i) {
        color_dc_htree_id[cht[i].id-1] = cht[i].ht_id>>4;
        color_ac_htree_id[cht[i].id-1] = cht[i].ht_id&0x0F;
    }

    int mcu_cnt_x = (width+mcu_size_x-1)/mcu_size_x;
    int mcu_cnt_y = (height+mcu_size_y-1)/mcu_size_y;
    int mcu_count = mcu_cnt_x*mcu_cnt_y;
    MCUDecoder *mcu_decoder[4];
    for(int i=0; i<color_ch_cnt; ++i) {
        mcu_decoder[i] = new MCUDecoder(
            dc_htree[color_dc_htree_id[i]],
            ac_htree[color_ac_htree_id[i]],
            qtable[color_qtable_id[i]]);
    }
    if(reset_interval == 0) reset_interval = mcu_count;
    int cnt = 0;
    BitReader bitReader(file);
    while(cnt<mcu_count) {
        for(int _cnt=0; _cnt<reset_interval; ++_cnt, ++cnt) {
            double block[4][4][4][8][8];
            for(int i=0; i<color_ch_cnt; ++i)
             for(int j=0; j<mcu_ch_cnt_y[i]; ++j)
              for(int k=0; k<mcu_ch_cnt_x[i]; ++k)
                  mcu_decoder[i]->decode(block[i][j][k], bitReader);
            int sy=cnt/mcu_cnt_x * mcu_size_y, sx = cnt%mcu_cnt_x * mcu_size_x;
            for(int i=0; i<mcu_size_y; ++i)
             for(int j=0; j<mcu_size_x; ++j) {
                int ny = sy+i, nx = sx+j;
                if( ny < height && nx < width) {
                    double ch[4];
                    for(int k=0; k<color_ch_cnt; ++k) {
                        int ky = i/(mcu_size_y/mcu_ch_cnt_y[k]/8);
                        int kx = j/(mcu_size_x/mcu_ch_cnt_x[k]/8);
                        ch[k] = block[k][ky/8][kx/8][ky%8][kx%8];
                    }
                    int ind = ny*width*3 + nx*3;
                    result[ind+0] = std::max(0., std::min(255., ch[0] + 1.402*ch[1] + 128));
                    result[ind+1] = std::max(0., std::min(255., ch[0] - 0.34414*ch[2] - 0.71414*ch[1] + 128));
                    result[ind+2] = std::max(0., std::min(255., ch[0] + 1.772*ch[2] + 128));
                }
            }
        }
        unsigned short code;
        READ_CODE(code, file);
        if(code == CODE_EOI) {
            fseek(file, -2, SEEK_CUR);
            break;
        }
        if(0xD0 <= code && code <= 0xD7) {
            for(int i=0; i<color_ch_cnt; ++i) {
                mcu_decoder[i]->resetDiff();
            }
        }
    }
    return true;
}

bool JPGDecoder::decode() {
    unsigned short code;
    READ_CODE(code, file);
    if(code != CODE_SOI) return false;
    while(true) {
        READ_CODE(code, file);
        if(code == CODE_EOI) break;
        read_segment();
        switch(code) {
            #define CASE(x) case CODE_##x: \
                if(!parse##x((x*)buf)) \
                    return false; \
                break;
            CASE(SOF0);
            CASE(DQT);
            CASE(DHT);
            CASE(DRI);
            CASE(SOS);
            default:
                if((code&0xFF00) != 0xFF00)
                    return false;
            #undef CASE
        }
    }
    if(code != CODE_EOI) return false;
    return true;
}

JPGDecoder::JPGDecoder(FILE *__file) {
    file = __file;
    reset_interval = 0;
}
byte *JPGDecoder::getBitmap() {
    return result;
}
int JPGDecoder::getWidth() {
    return width;
}
int JPGDecoder::getHeight() {
    return height;
}
