#include "MPEG1Decoder.h"

VLCTable::~VLCTable()
{
    if (m_lookup != NULL) delete [] m_lookup;
}

void VLCTable::setStream(BitStream &stream)
{
    m_stream = &stream;
}

void VLCTable::buildTable(std::vector<VLCTableEntry> &table)
{
    m_len = 0;
    for (VLCTableEntry &entry : table) {
        if (m_len < entry.len) m_len = entry.len;
    }
    m_lookup = new VLCTableEntry[1 << m_len];
    for (VLCTableEntry &entry : table) {
        unsigned start = entry.code << (m_len - entry.len);
        unsigned end = start | (1 << (m_len - entry.len)) - 1;
        for (unsigned i = start; i <= end; i++) m_lookup[i] = entry;
    }
}

int VLCTable::get()
{
    VLCTableEntry *entry = &m_lookup[m_stream->nextbits(m_len, false)];
    m_stream->nextbits(entry->len);
    return entry->value;
}

// tables

#define ADDENTRY(table, code, value) table.push_back({ bin2int(#code), binlen(#code), (value) })

static inline unsigned bin2int(const char *s)
{
    unsigned ret = 0;
    for (; *s == '0' || *s == '1'; s++) ret = ret << 1 | *s - '0';
    return ret;
}

static inline unsigned binlen(const char *s)
{
    unsigned ret = 0;
    for (; *s == '0' || *s == '1'; s++, ret++);
    return ret;
}

static inline void build_macro_addrinc(VLCTable &vlc)
{
    std::vector<VLCTableEntry> s_table;
    ADDENTRY(s_table, 1, 1);
    ADDENTRY(s_table, 011, 2);
    ADDENTRY(s_table, 010, 3);
    ADDENTRY(s_table, 0011, 4);
    ADDENTRY(s_table, 0010, 5);
    ADDENTRY(s_table, 00011, 6);
    ADDENTRY(s_table, 00010, 7);
    ADDENTRY(s_table, 0000111, 8);
    ADDENTRY(s_table, 0000110, 9);
    ADDENTRY(s_table, 00001011, 10);
    ADDENTRY(s_table, 00001010, 11);
    ADDENTRY(s_table, 00001001, 12);
    ADDENTRY(s_table, 00001000, 13);
    ADDENTRY(s_table, 00000111, 14);
    ADDENTRY(s_table, 00000110, 15);
    ADDENTRY(s_table, 0000010111, 16);
    ADDENTRY(s_table, 0000010110, 17);
    ADDENTRY(s_table, 0000010101, 18);
    ADDENTRY(s_table, 0000010100, 19);
    ADDENTRY(s_table, 0000010011, 20);
    ADDENTRY(s_table, 0000010010, 21);
    ADDENTRY(s_table, 00000100011, 22);
    ADDENTRY(s_table, 00000100010, 23);
    ADDENTRY(s_table, 00000100001, 24);
    ADDENTRY(s_table, 00000100000, 25);
    ADDENTRY(s_table, 00000011111, 26);
    ADDENTRY(s_table, 00000011110, 27);
    ADDENTRY(s_table, 00000011101, 28);
    ADDENTRY(s_table, 00000011100, 29);
    ADDENTRY(s_table, 00000011011, 30);
    ADDENTRY(s_table, 00000011010, 31);
    ADDENTRY(s_table, 00000011001, 32);
    ADDENTRY(s_table, 00000011000, 33);
    ADDENTRY(s_table, 00000001111, -1); // macroblock_stuffing
    ADDENTRY(s_table, 00000001000, -2); // macroblock_escape
    vlc.buildTable(s_table);
}

static inline void build_macro_I(VLCTable &vlc)
{
    std::vector<VLCTableEntry> s_table;
    ADDENTRY(s_table, 1, 0x01);
    ADDENTRY(s_table, 01, 0x11);
    vlc.buildTable(s_table);
}

static inline void build_macro_P(VLCTable &vlc)
{
    std::vector<VLCTableEntry> s_table;
    ADDENTRY(s_table, 1, 0x0a);
    ADDENTRY(s_table, 01, 0x02);
    ADDENTRY(s_table, 001, 0x08);
    ADDENTRY(s_table, 00011, 0x01);
    ADDENTRY(s_table, 00010, 0x1a);
    ADDENTRY(s_table, 00001, 0x12);
    ADDENTRY(s_table, 000001, 0x11);
    vlc.buildTable(s_table);
}

static inline void build_macro_B(VLCTable &vlc)
{
    std::vector<VLCTableEntry> s_table;
    ADDENTRY(s_table, 10, 0x0c);
    ADDENTRY(s_table, 11, 0x0e);
    ADDENTRY(s_table, 010, 0x04);
    ADDENTRY(s_table, 011, 0x06);
    ADDENTRY(s_table, 0010, 0x08);
    ADDENTRY(s_table, 0011, 0x0a);
    ADDENTRY(s_table, 00011, 0x01);
    ADDENTRY(s_table, 00010, 0x1e);
    ADDENTRY(s_table, 000011, 0x1a);
    ADDENTRY(s_table, 000010, 0x16);
    ADDENTRY(s_table, 000001, 0x11);
    vlc.buildTable(s_table);
}

static inline void build_cbp(VLCTable &vlc)
{
    std::vector<VLCTableEntry> s_table;
    ADDENTRY(s_table, 111, 60);
    ADDENTRY(s_table, 1101, 4);
    ADDENTRY(s_table, 1100, 8);
    ADDENTRY(s_table, 1011, 16);
    ADDENTRY(s_table, 1010, 32);
    ADDENTRY(s_table, 10011, 12);
    ADDENTRY(s_table, 10010, 48);
    ADDENTRY(s_table, 10001, 20);
    ADDENTRY(s_table, 10000, 40);
    ADDENTRY(s_table, 01111, 28);
    ADDENTRY(s_table, 01110, 44);
    ADDENTRY(s_table, 01101, 52);
    ADDENTRY(s_table, 01100, 56);
    ADDENTRY(s_table, 01011, 1);
    ADDENTRY(s_table, 01010, 61);
    ADDENTRY(s_table, 01001, 2);
    ADDENTRY(s_table, 01000, 62);
    ADDENTRY(s_table, 001111, 24);
    ADDENTRY(s_table, 001110, 36);
    ADDENTRY(s_table, 001101, 3);
    ADDENTRY(s_table, 001100, 63);
    ADDENTRY(s_table, 0010111, 5);
    ADDENTRY(s_table, 0010110, 9);
    ADDENTRY(s_table, 0010101, 17);
    ADDENTRY(s_table, 0010100, 33);
    ADDENTRY(s_table, 0010011, 6);
    ADDENTRY(s_table, 0010010, 10);
    ADDENTRY(s_table, 0010001, 18);
    ADDENTRY(s_table, 0010000, 34);
    ADDENTRY(s_table, 00011111, 7);
    ADDENTRY(s_table, 00011110, 11);
    ADDENTRY(s_table, 00011101, 19);
    ADDENTRY(s_table, 00011100, 35);
    ADDENTRY(s_table, 00011011, 13);
    ADDENTRY(s_table, 00011010, 49);
    ADDENTRY(s_table, 00011001, 21);
    ADDENTRY(s_table, 00011000, 41);
    ADDENTRY(s_table, 00010111, 14);
    ADDENTRY(s_table, 00010110, 50);
    ADDENTRY(s_table, 00010101, 22);
    ADDENTRY(s_table, 00010100, 42);
    ADDENTRY(s_table, 00010011, 15);
    ADDENTRY(s_table, 00010010, 51);
    ADDENTRY(s_table, 00010001, 23);
    ADDENTRY(s_table, 00010000, 43);
    ADDENTRY(s_table, 00001111, 25);
    ADDENTRY(s_table, 00001110, 37);
    ADDENTRY(s_table, 00001101, 26);
    ADDENTRY(s_table, 00001100, 38);
    ADDENTRY(s_table, 00001011, 29);
    ADDENTRY(s_table, 00001010, 45);
    ADDENTRY(s_table, 00001001, 53);
    ADDENTRY(s_table, 00001000, 57);
    ADDENTRY(s_table, 00000111, 30);
    ADDENTRY(s_table, 00000110, 46);
    ADDENTRY(s_table, 00000101, 54);
    ADDENTRY(s_table, 00000100, 58);
    ADDENTRY(s_table, 000000111, 31);
    ADDENTRY(s_table, 000000110, 47);
    ADDENTRY(s_table, 000000101, 55);
    ADDENTRY(s_table, 000000100, 59);
    ADDENTRY(s_table, 000000011, 27);
    ADDENTRY(s_table, 000000010, 39);
    vlc.buildTable(s_table);
}

static inline void build_mv(VLCTable &vlc)
{
    std::vector<VLCTableEntry> s_table;
    ADDENTRY(s_table, 00000011001, -16);
    ADDENTRY(s_table, 00000011011, -15);
    ADDENTRY(s_table, 00000011101, -14);
    ADDENTRY(s_table, 00000011111, -13);
    ADDENTRY(s_table, 00000100001, -12);
    ADDENTRY(s_table, 00000100011, -11);
    ADDENTRY(s_table, 0000010011, -10);
    ADDENTRY(s_table, 0000010101, -9);
    ADDENTRY(s_table, 0000010111, -8);
    ADDENTRY(s_table, 00000111, -7);
    ADDENTRY(s_table, 00001001, -6);
    ADDENTRY(s_table, 00001011, -5);
    ADDENTRY(s_table, 0000111, -4);
    ADDENTRY(s_table, 00011, -3);
    ADDENTRY(s_table, 0011, -2);
    ADDENTRY(s_table, 011, -1);
    ADDENTRY(s_table, 1, 0);
    ADDENTRY(s_table, 010, 1);
    ADDENTRY(s_table, 0010, 2);
    ADDENTRY(s_table, 00010, 3);
    ADDENTRY(s_table, 0000110, 4);
    ADDENTRY(s_table, 00001010, 5);
    ADDENTRY(s_table, 00001000, 6);
    ADDENTRY(s_table, 00000110, 7);
    ADDENTRY(s_table, 0000010110, 8);
    ADDENTRY(s_table, 0000010100, 9);
    ADDENTRY(s_table, 0000010010, 10);
    ADDENTRY(s_table, 00000100010, 11);
    ADDENTRY(s_table, 00000100000, 12);
    ADDENTRY(s_table, 00000011110, 13);
    ADDENTRY(s_table, 00000011100, 14);
    ADDENTRY(s_table, 00000011010, 15);
    ADDENTRY(s_table, 00000011000, 16);
    vlc.buildTable(s_table);
}

static inline void build_dct_dc_luma(VLCTable &vlc)
{
    std::vector<VLCTableEntry> s_table;
    ADDENTRY(s_table, 100, 0);
    ADDENTRY(s_table, 00, 1);
    ADDENTRY(s_table, 01, 2);
    ADDENTRY(s_table, 101, 3);
    ADDENTRY(s_table, 110, 4);
    ADDENTRY(s_table, 1110, 5);
    ADDENTRY(s_table, 11110, 6);
    ADDENTRY(s_table, 111110, 7);
    ADDENTRY(s_table, 1111110, 8);
    vlc.buildTable(s_table);
}

static inline void build_dct_dc_chroma(VLCTable &vlc)
{
    std::vector<VLCTableEntry> s_table;
    ADDENTRY(s_table, 00, 0);
    ADDENTRY(s_table, 01, 1);
    ADDENTRY(s_table, 10, 2);
    ADDENTRY(s_table, 110, 3);
    ADDENTRY(s_table, 1110, 4);
    ADDENTRY(s_table, 11110, 5);
    ADDENTRY(s_table, 111110, 6);
    ADDENTRY(s_table, 1111110, 7);
    ADDENTRY(s_table, 11111110, 8);
    vlc.buildTable(s_table);
}

static inline void build_dct_coeff_common(std::vector<VLCTableEntry> &s_table)
{
    std::vector<VLCTableEntry> table;

    ADDENTRY(table, 0110, 1 << 8 | 1);
    ADDENTRY(table, 01000, 0 << 8 | 2);
    ADDENTRY(table, 01010, 2 << 8 | 1);
    ADDENTRY(table, 001010, 0 << 8 | 3);
    ADDENTRY(table, 001110, 3 << 8 | 1);
    ADDENTRY(table, 001100, 4 << 8 | 1);
    ADDENTRY(table, 0001100, 1 << 8 | 2);
    ADDENTRY(table, 0001110, 5 << 8 | 1);
    ADDENTRY(table, 0001010, 6 << 8 | 1);
    ADDENTRY(table, 0001000, 7 << 8 | 1);
    ADDENTRY(table, 00001100, 0 << 8 | 4);
    ADDENTRY(table, 00001000, 2 << 8 | 2);
    ADDENTRY(table, 00001110, 8 << 8 | 1);
    ADDENTRY(table, 00001010, 9 << 8 | 1);
    ADDENTRY(table, 001001100, 0 << 8 | 5);
    ADDENTRY(table, 001000010, 0 << 8 | 6);
    ADDENTRY(table, 001001010, 1 << 8 | 3);
    ADDENTRY(table, 001001000, 3 << 8 | 2);
    ADDENTRY(table, 001001110, 10 << 8 | 1);
    ADDENTRY(table, 001000110, 11 << 8 | 1);
    ADDENTRY(table, 001000100, 12 << 8 | 1);
    ADDENTRY(table, 001000000, 13 << 8 | 1);
    ADDENTRY(table, 00000010100, 0 << 8 | 7);
    ADDENTRY(table, 00000011000, 1 << 8 | 4);
    ADDENTRY(table, 00000010110, 2 << 8 | 3);
    ADDENTRY(table, 00000011110, 4 << 8 | 2);
    ADDENTRY(table, 00000010010, 5 << 8 | 2);
    ADDENTRY(table, 00000011100, 14 << 8 | 1);
    ADDENTRY(table, 00000011010, 15 << 8 | 1);
    ADDENTRY(table, 00000010000, 16 << 8 | 1);
    ADDENTRY(table, 0000000111010, 0 << 8 | 8);
    ADDENTRY(table, 0000000110000, 0 << 8 | 9);
    ADDENTRY(table, 0000000100110, 0 << 8 | 10);
    ADDENTRY(table, 0000000100000, 0 << 8 | 11);
    ADDENTRY(table, 0000000110110, 1 << 8 | 5);
    ADDENTRY(table, 0000000101000, 2 << 8 | 4);
    ADDENTRY(table, 0000000111000, 3 << 8 | 3);
    ADDENTRY(table, 0000000100100, 4 << 8 | 3);
    ADDENTRY(table, 0000000111100, 6 << 8 | 2);
    ADDENTRY(table, 0000000101010, 7 << 8 | 2);
    ADDENTRY(table, 0000000100010, 8 << 8 | 2);
    ADDENTRY(table, 0000000111110, 17 << 8 | 1);
    ADDENTRY(table, 0000000110100, 18 << 8 | 1);
    ADDENTRY(table, 0000000110010, 19 << 8 | 1);
    ADDENTRY(table, 0000000101110, 20 << 8 | 1);
    ADDENTRY(table, 0000000101100, 21 << 8 | 1);
    ADDENTRY(table, 00000000110100, 0 << 8 | 12);
    ADDENTRY(table, 00000000110010, 0 << 8 | 13);
    ADDENTRY(table, 00000000110000, 0 << 8 | 14);
    ADDENTRY(table, 00000000101110, 0 << 8 | 15);
    ADDENTRY(table, 00000000101100, 1 << 8 | 6);
    ADDENTRY(table, 00000000101010, 1 << 8 | 7);
    ADDENTRY(table, 00000000101000, 2 << 8 | 5);
    ADDENTRY(table, 00000000100110, 3 << 8 | 4);
    ADDENTRY(table, 00000000100100, 5 << 8 | 3);
    ADDENTRY(table, 00000000100010, 9 << 8 | 2);
    ADDENTRY(table, 00000000100000, 10 << 8 | 2);
    ADDENTRY(table, 00000000111110, 22 << 8 | 1);
    ADDENTRY(table, 00000000111100, 23 << 8 | 1);
    ADDENTRY(table, 00000000111010, 24 << 8 | 1);
    ADDENTRY(table, 00000000111000, 25 << 8 | 1);
    ADDENTRY(table, 00000000110110, 26 << 8 | 1);
    ADDENTRY(table, 000000000111110, 0 << 8 | 16);
    ADDENTRY(table, 000000000111100, 0 << 8 | 17);
    ADDENTRY(table, 000000000111010, 0 << 8 | 18);
    ADDENTRY(table, 000000000111000, 0 << 8 | 19);
    ADDENTRY(table, 000000000110110, 0 << 8 | 20);
    ADDENTRY(table, 000000000110100, 0 << 8 | 21);
    ADDENTRY(table, 000000000110010, 0 << 8 | 22);
    ADDENTRY(table, 000000000110000, 0 << 8 | 23);
    ADDENTRY(table, 000000000101110, 0 << 8 | 24);
    ADDENTRY(table, 000000000101100, 0 << 8 | 25);
    ADDENTRY(table, 000000000101010, 0 << 8 | 26);
    ADDENTRY(table, 000000000101000, 0 << 8 | 27);
    ADDENTRY(table, 000000000100110, 0 << 8 | 28);
    ADDENTRY(table, 000000000100100, 0 << 8 | 29);
    ADDENTRY(table, 000000000100010, 0 << 8 | 30);
    ADDENTRY(table, 000000000100000, 0 << 8 | 31);
    ADDENTRY(table, 0000000000110000, 0 << 8 | 32);
    ADDENTRY(table, 0000000000101110, 0 << 8 | 33);
    ADDENTRY(table, 0000000000101100, 0 << 8 | 34);
    ADDENTRY(table, 0000000000101010, 0 << 8 | 35);
    ADDENTRY(table, 0000000000101000, 0 << 8 | 36);
    ADDENTRY(table, 0000000000100110, 0 << 8 | 37);
    ADDENTRY(table, 0000000000100100, 0 << 8 | 38);
    ADDENTRY(table, 0000000000100010, 0 << 8 | 39);
    ADDENTRY(table, 0000000000100000, 0 << 8 | 40);
    ADDENTRY(table, 0000000000111110, 1 << 8 | 8);
    ADDENTRY(table, 0000000000111100, 1 << 8 | 9);
    ADDENTRY(table, 0000000000111010, 1 << 8 | 10);
    ADDENTRY(table, 0000000000111000, 1 << 8 | 11);
    ADDENTRY(table, 0000000000110110, 1 << 8 | 12);
    ADDENTRY(table, 0000000000110100, 1 << 8 | 13);
    ADDENTRY(table, 0000000000110010, 1 << 8 | 14);
    ADDENTRY(table, 00000000000100110, 1 << 8 | 15);
    ADDENTRY(table, 00000000000100100, 1 << 8 | 16);
    ADDENTRY(table, 00000000000100010, 1 << 8 | 17);
    ADDENTRY(table, 00000000000100000, 1 << 8 | 18);
    ADDENTRY(table, 00000000000101000, 6 << 8 | 3);
    ADDENTRY(table, 00000000000110100, 11 << 8 | 2);
    ADDENTRY(table, 00000000000110010, 12 << 8 | 2);
    ADDENTRY(table, 00000000000110000, 13 << 8 | 2);
    ADDENTRY(table, 00000000000101110, 14 << 8 | 2);
    ADDENTRY(table, 00000000000101100, 15 << 8 | 2);
    ADDENTRY(table, 00000000000101010, 16 << 8 | 2);
    ADDENTRY(table, 00000000000111110, 27 << 8 | 1);
    ADDENTRY(table, 00000000000111100, 28 << 8 | 1);
    ADDENTRY(table, 00000000000111010, 29 << 8 | 1);
    ADDENTRY(table, 00000000000111000, 30 << 8 | 1);
    ADDENTRY(table, 00000000000110110, 31 << 8 | 1);

    for (VLCTableEntry entry : table) {
        s_table.push_back(entry);
        entry.code |= 1;
        entry.value = entry.value & 0xff00 | -(entry.value & 0xff) & 0xff;
        s_table.push_back(entry);
    }

    ADDENTRY(s_table, 000001, -2); // escape
}

static inline void build_dct_coeff_first(VLCTable &vlc)
{
    std::vector<VLCTableEntry> s_table;
    build_dct_coeff_common(s_table);
    ADDENTRY(s_table, 10, 0x0001);
    ADDENTRY(s_table, 11, 0x00ff);
    vlc.buildTable(s_table);
}

static inline void build_dct_coeff_next(VLCTable &vlc)
{
    std::vector<VLCTableEntry> s_table;
    build_dct_coeff_common(s_table);
    ADDENTRY(s_table, 10, -1); // EOB
    ADDENTRY(s_table, 110, 0x0001);
    ADDENTRY(s_table, 111, 0x00ff);
    vlc.buildTable(s_table);
}

VLCTable Tables::macro_addrinc;
VLCTable Tables::macro_I;
VLCTable Tables::macro_P;
VLCTable Tables::macro_B;
VLCTable Tables::cbp;
VLCTable Tables::mv;
VLCTable Tables::dct_dc_luma;
VLCTable Tables::dct_dc_chroma;
VLCTable Tables::dct_coeff_first;
VLCTable Tables::dct_coeff_next;

#define INIT_TABLE(table) { table.setStream(stream); build_##table(table); }

void Tables::initTables(BitStream &stream)
{
    INIT_TABLE(macro_addrinc)
    INIT_TABLE(macro_I)
    INIT_TABLE(macro_P)
    INIT_TABLE(macro_B)
    INIT_TABLE(cbp)
    INIT_TABLE(mv)
    INIT_TABLE(dct_dc_luma)
    INIT_TABLE(dct_dc_chroma)
    INIT_TABLE(dct_coeff_first)
    INIT_TABLE(dct_coeff_next)
}