#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <queue>
#include <algorithm>
#include <thread>
#include <mutex>
#include <unistd.h>

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

class BitStream {
private:
    static const int BUFSIZE = 4096;
    uint8_t buf[BUFSIZE];
    int pos_byte, pos_bit;
    int pos_file;
    FILE *fp = NULL;
public:
    ~BitStream();
    void open(const char *filename);
    uint32_t nextbits(int count, bool pop = true);
    void align();
    int getpos();
    void setpos(int newpos);
};

struct Pixel {
    uint8_t r, g, b;
    Pixel(uint8_t i_r, uint8_t i_g, uint8_t i_b) : r(i_r), g(i_g), b(i_b) {}
    Pixel() : Pixel(0, 0, 0) {}
    inline void diff(int dr, int dg, int db);
};

struct VLCTableEntry {
    unsigned code, len;
    int value;
};

class VLCTable {
private:
    BitStream *m_stream;
    unsigned m_len = 0;
    VLCTableEntry *m_lookup = NULL;
public:
    ~VLCTable();
    void setStream(BitStream &stream);
    void buildTable(std::vector<VLCTableEntry> &table);
    int get();
};

struct Tables {
    static VLCTable macro_addrinc;
    static VLCTable macro_I;
    static VLCTable macro_P;
    static VLCTable macro_B;
    static VLCTable cbp;
    static VLCTable mv;
    static VLCTable dct_dc_luma;
    static VLCTable dct_dc_chroma;
    static VLCTable dct_coeff_first;
    static VLCTable dct_coeff_next;
    static void initTables(BitStream &stream);
};

struct PictureData {
    int temp_ref;
    char type;
    bool full_pel_forward;
    int forward_f, forward_r_size;
    bool full_pel_backward;
    int backward_f, backward_r_size;
    Pixel *buffer;
};

struct SliceData {
    int vpos;
    int q_scale;
    int last_mb_addr;
    int last_intra_addr;
    int dc_predictor[3]; // Y, Cb, Cr
    int recon_right_for_prev, recon_down_for_prev;
    int recon_right_back_prev, recon_down_back_prev;
};

struct MacroblockData {
    int addr;
    bool quant;
    bool motion_forward;
    bool motion_backward;
    bool pattern;
    bool intra;
    int q_scale;
    int block[6][64];
    int recon_right_for, recon_down_for;
    int recon_right_back, recon_down_back;
};

struct MPEG1Data {
    BitStream stream;
    uint32_t next_start_code;
    int width;
    int height;
    int width_mb;
    int height_mb;
    int width_orig;
    int height_orig;
    double pixel_ar;
    double fps;
    uint8_t q_intra[64];
    uint8_t q_nonintra[64];
    std::queue<Pixel*> frames;
    std::mutex mtx_frames;
    PictureData cur_picture;
    SliceData cur_slice;
    MacroblockData cur_mb;
    Pixel *forward_ref = NULL, *backward_ref = NULL;
    bool verbose = true;
    bool terminate = false;
    bool seeking = false;
    bool was_seeking = false;
    bool decoding = false;
    std::vector<int> index;
    int last_frame_id = 0;
    int base_frame_id = 0;
    MPEG1Data();
};

void decodeHeader(MPEG1Data &mpg);
void decodeGOP(MPEG1Data &mpg);
void decodeGOPIndex(MPEG1Data &mpg);

void idct(int *b);

// utils

inline int sign(int n)
{
    if (n > 0) return 1;
    if (n < 0) return -1;
    return 0;
}

template<typename T>
inline T clip(T n, T lower, T upper)
{
    return std::min(std::max(n, lower), upper);
}

inline void Pixel::diff(int dr, int dg, int db)
{
    r = clip(r + dr, 0, 255);
    g = clip(g + dg, 0, 255);
    b = clip(b + db, 0, 255);
}

inline void trim(std::string &str)
{
    int pos;
    pos = str.find_first_not_of(" \t\r\n");
    if (pos != std::string::npos) str.substr(pos).swap(str);
    else { str = ""; return; }
    pos = str.find_last_not_of(" \t\r\n");
    if (pos != std::string::npos) str.substr(0, pos + 1).swap(str);
}
