#ifndef _JPG_DECODER_H_
#define _JPG_DECODER_H_
#include "jfif_header.h"
#include "huffman_decode.h"
class JPGDecoder {
private:
    FILE *file;
    char buf[65536];
    byte *result;
    int height, width, color_ch_cnt;
    int reset_interval;
    int mcu_ch_cnt_x[4], mcu_ch_cnt_y[4];
    int color_qtable_id[4];
    int color_dc_htree_id[4];
    int color_ac_htree_id[4];
    int qtable[4][64];
    int mcu_size_x, mcu_size_y;
    HuffmanTree *dc_htree[16], *ac_htree[16];

    inline void read_segment();
    bool parseDQT(DQT *dqt);
    bool parseDHT(DHT *dht);
    bool parseSOF0(SOF0 *sof0);
    bool parseDRI(DRI *dri);
    bool parseSOS(SOS *sos);

public:
    JPGDecoder(FILE *__file);
    bool decode();
    byte *getBitmap();
    int getWidth();
    int getHeight();
};
#endif
