#pragma once

#include "bitReader.h"

#include <bitset>
#include <cassert>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <map>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>

#include "CImg.h"
using namespace cimg_library_suffixed;
using namespace cimg_library;

#define INF 2147483647
//#define DEBUG
// SPEEDUP: int -> char, no copy vector
typedef unsigned char byte;

template <typename T>
void toZero(T &e) { // https://stackoverflow.com/a/17625427/2678970
  static T dummy_zero_object;
  e = dummy_zero_object;
}

struct node;
struct BlockHeader;

const int scan[][8] = {{0, 1, 5, 6, 14, 15, 27, 28},
                       {2, 4, 7, 13, 16, 26, 29, 42},
                       {3, 8, 12, 17, 25, 30, 41, 43},
                       {9, 11, 18, 24, 31, 40, 44, 53},
                       {10, 19, 23, 32, 39, 45, 52, 54},
                       {20, 22, 33, 38, 46, 51, 55, 60},
                       {21, 34, 37, 47, 50, 56, 59, 61},
                       {35, 36, 48, 49, 57, 58, 62, 63}}; // zigzag order

const int default_intra_quant_matrix[8][8] = {
    {8, 16, 19, 22, 26, 27, 29, 34},  {16, 16, 22, 24, 27, 29, 34, 37},
    {19, 22, 26, 27, 29, 34, 34, 38}, {22, 22, 26, 27, 29, 34, 37, 40},
    {22, 26, 27, 29, 32, 35, 40, 48}, {26, 27, 29, 32, 35, 40, 48, 58},
    {26, 27, 29, 34, 38, 46, 56, 69}, {27, 29, 35, 38, 46, 56, 69, 83},
};
const int default_non_intra_quant_matrix[8][8] = {
    {16, 16, 16, 16, 16, 16, 16, 16}, {16, 16, 16, 16, 16, 16, 16, 16},
    {16, 16, 16, 16, 16, 16, 16, 16}, {16, 16, 16, 16, 16, 16, 16, 16},
    {16, 16, 16, 16, 16, 16, 16, 16}, {16, 16, 16, 16, 16, 16, 16, 16},
    {16, 16, 16, 16, 16, 16, 16, 16}, {16, 16, 16, 16, 16, 16, 16, 16},
};

/*
Y-matrix must have an even number of rows and columns
Cr and Cb matrices are one half the size of the Y-matrix in both horizontal and
vertical dimensions
 */
struct YCbCrBuffer {
  int serial;
  double y[768][576];
  double cb[384][288];
  double cr[384][288];
};

struct OriginalBlock {
  int past_intra_addr; // absolute position of the last non-skipped macroblock
  // At the start of a slice previous_macroblock_address is reset as follows:
  // previous_macroblock_address=(slice_vertical_position-1)*mb_width-1;
  int macroblock_addr; // absolute position of the current macroblock. The
                       // macroblock_address of the top-left macroblock is zero

  int recon_right_for_prev;
  int recon_right_for;
  int recon_right_back;
  int recon_right_back_prev;
  int recon_down_for_prev;
  int recon_down_for;
  int recon_down_back_prev;
  int recon_down_back;

  double dct_dc_y_past;
  double dct_dc_cb_past;
  double dct_dc_cr_past;
  double recon[8][8];

  byte slice_vert_pos;
  // The slice_vertical_position differs from mb_row by one
  int macroBlockWidth;
  int macroBlockHeight;

  void idct();
  void recoverMotionVector(int f, int code, int r, int &recon_prev,
                           bool full_pel_vector,
                           int &recon_for) {
    int complement_r;
    if (f == 1 || code == 0) {
      complement_r = 0;
    } else {
      complement_r = f - 1 - r;
    }

    int little = code * f;
    int big;
    if (little == 0) {
      big = 0;
    } else {
      if (little > 0) {
        little = little - complement_r;
        big = little - 32 * f;
      } else {
        little = little + complement_r;
        big = little + 32 * f;
      }
    }

    int max = (16 * f) - 1;
    int min = (-16 * f);
    int new_vector = recon_prev + little;
    if (new_vector <= max && new_vector >= min)
      recon_for = recon_prev + little;
    else
      recon_for = recon_prev + big;
    recon_prev = recon_for;
    if (full_pel_vector)
      recon_for = recon_for << 1;
  }
  OriginalBlock(){};
  void reset() {
    /*
      The predictors dct_dc_y_past, dct_dc_cb_past and dct_dc_cr_past shall all
      be
      reset at the start of a slice and at non-intra-coded macroblocks
      (including skipped macroblocks) to the value 1024 (128*8).
    */
    past_intra_addr = -2;
    macroblock_addr = (slice_vert_pos - 1) * macroBlockWidth - 1;
    epr("reset macroblock_addr = %d: slice vert pos = %d, macroBlockWidth %d\n",
        macroblock_addr, slice_vert_pos, macroBlockWidth);
    recon_right_for_prev = recon_down_for_prev = 0;
    recon_right_back_prev = recon_down_back_prev = 0;
    dct_dc_y_past = dct_dc_cb_past = dct_dc_cr_past = 1024;
  }
  void resetRecon() {
    recon_right_for_prev = recon_down_for_prev = 0;
    recon_right_back_prev = recon_down_back_prev = 0;
  }
};

struct Motion {
  int horizontal_forward_code;  // 1~11
  int horizontal_forward_r;     // 1-6
  int vertical_forward_code;    // 1-11
  int vertical_forward_r;       // 1-6
  int horizontal_backward_code; // 1-11
  int horizontal_backward_r;    // 1-6
  int vertical_backward_code;   // 1-11
  int vertical_backward_r;      // 1-6
  void print() {
    epr("[MOTION HEADER]\n");
    epr("forward %d %d %d %d\n", horizontal_forward_r, horizontal_forward_code,
        vertical_forward_r, vertical_forward_code);
    epr("backward %d %d %d %d\n", horizontal_backward_r,
        horizontal_backward_code, vertical_backward_r, vertical_backward_code);
  }
  void reset() { toZero(*this); }
};

struct BlockHeader {
  int dct_dc_size_luminance;
  int dct_dc_differential;
  int dct_dc_size_chrominance;
  int dct_coeff_first;
  int dct_coeff_next;

  int dct_zz[64];

  void print() {
    epr("[BLOCK HEADER]: ");
    epr("lumi %d diff %d, chromi %d\n", dct_dc_size_luminance,
        dct_dc_differential, dct_dc_size_chrominance);
    epr("dct_zz:\n");
    for(int i= 0; i < 8; i++){
        for(int j= 0; j < 8; j++){
            epr("%d ", dct_zz[i*8+j]);
        }
        epr("\n");
    }
  }
  void reset() { toZero(*this); }
};

struct MacroBlockHeader {
  int address_increment; // 1-11
  int type;              // 1-6bits, value = 5bits
  Motion motion = {};
  int coded_block_pattern; // 3-9
  bool isQuant, isMotionForward, isMotionBackward, isPattern, isIntra;
  void decodeProperties() {
    int base = 1;
    isQuant = type & base;
    base <<= 1;
    isMotionForward = type & base;
    base <<= 1;
    isMotionBackward = type & base;
    base <<= 1;
    isPattern = type & base;
    base <<= 1;
    isIntra = type & base;
  }
  void print(int i) {
    epr("[MACRO HEADER %d]\n", i);
    epr("address increment %d, block pattern %d\n", address_increment,
        coded_block_pattern);
    epr("isQuant %d isMotionForward %d isMotionBackward %d isPattern %d "
        "isIntra %d\n",
        isQuant, isMotionForward, isMotionBackward, isPattern, isIntra);
    motion.print();
  }
  void reset() {
    toZero(motion);
    toZero(*this);
  }
};

struct SliceHeader {
  byte vertical_position; // vertical position in macroblock units of the first
                          // macroblock in the slice
  // slice_vertical_position of the first row of macroblocks is one
  byte quantizer_scale; // scale the reconstruction level of the retrieved DCT
                        // coefficient levels.

  void read(BitReader *br) {
    br->checkAndDiscard(start_code); // 24bit
    vertical_position = br->getBits(8);
    quantizer_scale = br->getBits(5);
    while (br->checkBit(true)) {
      br->discard(1); // extra_bit_slice "1";
      br->discard(8); // extra_information_slice 8;
    }
    br->checkAndDiscard("0"); // extra_bit_slice "0";
    print();
  }
  void print() {
    epr("[SLICE HEADER]\n");
    epr("vertical_position %d, quantizer_scale %d\n", vertical_position,
        quantizer_scale);
  }
  void reset() { toZero(*this); }
};

struct PictureHeader {
  uint serial; // modulo 1024, initial 0, display order
  uint type;
  uint VBVdelay;

  bool forward_vector; // full_pel
  byte forward_fcode;
  byte forward_r_size;
  byte forward_f;

  bool backward_vector;
  byte backward_fcode;
  byte backward_r_size;
  byte backward_f;

  char typec[10];
  PictureHeader() {}
  bool isI() { return type == 1; }
  bool isP() { return type == 2; }
  bool isB() { return type == 3; }
  bool isD() { return type == 4; }
  void read(BitReader *br) {
    br->checkAndDiscard(picture_start_code);
    serial = br->getBits(10);
    type = br->getBits(3);
    VBVdelay = br->getBits(16); // decoder do not have to use this

    if (isP() || isB()) { // forward
      forward_vector = br->getBits(1);
      forward_fcode = br->getBits(3);
      forward_r_size = forward_fcode - 1;
      forward_f = 1 << forward_r_size;
    }
    if (isB()) { // backward
      backward_vector = br->getBits(1);
      backward_fcode = br->getBits(3);
      backward_r_size = backward_fcode - 1;
      backward_f = 1 << backward_r_size;
    }
    while (br->checkBit(true)) {
      br->discard(1); // extra_bit_picture
      br->discard(8); // extra_information_picture
    }
    br->checkAndDiscard("0");

    br->readExtension();
    print();
  }
  void print() {
    epr("[PICTURE HEADER]\n");
    fprintf(stderr, "serial %d type %c\n", serial, typec[type]);
    epr("VBVdelay %d vector (%d, %d) fcode (%d, %d)\n", VBVdelay,
        forward_vector, backward_vector, forward_fcode, backward_fcode);
  }
  void reset() {
    toZero(*this);
    strcpy(typec, "0IPBD");
  }
};

struct GOPHeader {
  // time
  bool isDropFrame;
  /*
It may be set to "1" only if the picture rate is 29.97Hz.  If it is "0" then
pictures are counted assuming rounding to the nearest integral number of
pictures per second, for example 29.97Hz would be rounded to and counted as
30Hz.  If it is "1" then picture numbers 0 and 1 at the start of each minute,
except minutes 0, 10, 20, 30, 40, 50 are omitted from the count.
*/
  uint hours;
  uint minutes;
  uint seconds;
  uint frames;

  bool isClosed;
  /*
    TODO: clean buffer if it's 1
    set to "1" if the group of pictures has been encoded without prediction
    vectors pointing to the the previous GOP.
   */
  bool isBroken;
  /*
    if isbroken, do not output
   */

  void read(BitReader *br) {
    br->checkAndDiscard(group_start_code);
    isDropFrame = br->getBit();
    hours = br->getBits(5);
    minutes = br->getBits(6);
    br->checkAndDiscard("1");
    seconds = br->getBits(6);
    frames = br->getBits(6);
    isClosed = br->getBit();
    isBroken = br->getBit();

    br->readExtension();
    assert(!isBroken);
    print();
  }
  void print() {
    epr("[GOP HEADER]\n");
    epr("isDropFrame %d, isClosed %d broken GOP %d\n", isDropFrame, isClosed,
        isBroken);
    epr("%d hour %d minutes %d seconds\n", hours, minutes, seconds);
    epr("%d frames\n", frames);
  }
  void reset() { toZero(*this); }
};

struct SequenceHeader { //
  uint h;               // 12
  uint v;               // 12
  uint aspectRatio; // 4//TODO: four-bit integer defined in the following table
  uint frameRate;   // 4
  uint bitRate;     // 20// 400 bits/second, rounded upwards
  // 3FFFF (11 1111 1111 1111 1111) identifies variable bit rate operation.
  uint vbvBufferSize; // 10//B = 16 * 1024 * vbv_buffer_size
  // where B is the minimum VBV buffer size in bits required to decode the
  // sequence
  bool constrained_parameters;
  // set to "1" if the following data elements meet the following constraints
  bool load_intra_quantizer_matrix;
  // If it is set to "0" then the default values defined below are used
  bool load_non_intra_quantizer_matrix;

  byte intra_quantizer_matrix[8][8];
  byte nonintra_quantizer_matrix[8][8];

  void read(BitReader *br) {
    br->checkAndDiscard(sequence_start_code);
    h = br->getBits(12);
    v = br->getBits(12);
    aspectRatio = br->getBits(4);
    frameRate = br->getBits(4);
    bitRate = br->getBits(18);
    br->checkAndDiscard("1");
    vbvBufferSize = br->getBits(10);
    constrained_parameters = br->getBit();

    load_intra_quantizer_matrix = br->getBit();
    if (load_intra_quantizer_matrix)
      for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
          intra_quantizer_matrix[i][j] = br->getBits(8);
    else
      for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
          intra_quantizer_matrix[i][j] = default_intra_quant_matrix[i][j];
    assert(intra_quantizer_matrix[0][0] == 8);

    load_non_intra_quantizer_matrix = br->getBit();
    if (load_non_intra_quantizer_matrix)
      for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
          nonintra_quantizer_matrix[i][j] = br->getBits(8);
    else
      for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
          nonintra_quantizer_matrix[i][j] =
              default_non_intra_quant_matrix[i][j];

    br->readExtension();

    print();
  }
  void print() {
    epr("[SEQUENCE HEADER]\n");
    epr("horizontal %d vertical %d\n", h, v);
    epr("aspectRatio %d frameRate %d\n", aspectRatio, frameRate);
    epr("bitrate %d vbvBufferSize %d\n", bitRate, vbvBufferSize);
    epr("constrained_parameters %d, load_intra_quantizer_matrix %d, "
        "load_non_intra_quantizer_matrix %d\n",
        constrained_parameters, load_intra_quantizer_matrix,
        load_non_intra_quantizer_matrix);
  }
  void reset() { toZero(*this); }
};

struct node {
  int value;
  node *c[2];
  node() {
    value = -INF;
    c[0] = c[1] = NULL;
  }
  bool isLeaf() { return value != -INF; }
  node *getChild(int i) {
    if (c[i] == NULL)
      c[i] = new node();
    return c[i];
  }
  void setValue(int v) {
    value = v;
    assert(c[0] == NULL);
    assert(c[1] == NULL);
    // epr("set value %d\n", v);
  }
};

class HuffmanTree { // TODO: move to new file
public:
  BitReader &bitReader;
  node *root;
  HuffmanTree();
  HuffmanTree(char *fileName, BitReader &bitReader) : bitReader(bitReader) {
    FILE *f = fopen(fileName, "r");
    root = new node();
  }
  virtual void getValue(int &run, int &level){};
  void getValue(int &value) {
    node *nown = root;
    while (nown != NULL && !nown->isLeaf()) {
      if (bitReader.nextBit())
        nown = nown->c[1];
      else
        nown = nown->c[0];
      assert(nown != NULL);
    }
    value = nown->value;
  }
  void print();
};

class ValueTree : public HuffmanTree {
public:
  ValueTree(char *fileName, BitReader &bitReader)
      : HuffmanTree(fileName, bitReader) {
    FILE *f = fopen(fileName, "r");
    root = new node();
    char path[1000];
    int value;
    while (fscanf(f, "%s %d\n", path, &value) == 2) {
      node *nown = root;
      for (size_t i = 0; i < strlen(path); i++) {
        if (path[i] == '0')
          nown = nown->getChild(0);
        else if (path[i] == '1')
          nown = nown->getChild(1);
        else
          assert(false);
      }
      nown->setValue(value);
    }
  }
};

class RunLevelTree : public HuffmanTree {
public:
  RunLevelTree(char *fileName, BitReader &bitReader)
      : HuffmanTree(fileName, bitReader) {
    FILE *f = fopen(fileName, "r");
    root = new node();
    char path[1000];
    int run, level;
    while (fscanf(f, "%s %d %d\n", path, &run, &level) == 3) {
      int value = run * 100 + level;
      node *nown = root;
      for (size_t i = 0; i < strlen(path); i++) {
        if (path[i] == '0')
          nown = nown->getChild(0);
        else if (path[i] == '1')
          nown = nown->getChild(1);
        else
          assert(false);
      }
      // epr("set runlevel %s = %d, %d\n", path, run, level);
      nown->setValue(value);
    }
  }

  void getValue(int &run, int &level) {
    if (bitReader.checkBits("000001")) {
      epr("ESCAPE\n");
      bitReader.discard(6);
      run = bitReader.getBits(6);
      int tmp = bitReader.getBits(8);
      /*
        20-bit fixed length code (-127 <= level <= 127) or as a 28-bit fixed
        length code (-255 <= level <= -128, 128 <= level <= 255)
       */
      if (tmp == 0x00) { // level=128~255
        level = bitReader.getBits(8);
      } else if (tmp == 0x80) { // level=-128~-255
        level = bitReader.getBits(8) - 256;
      } else { // level=-127~127
        level = char(tmp);
        assert(level != 0);
      }
      return;
    }
    node *nown = root;
    while (nown != NULL && !nown->isLeaf()) {
      if (bitReader.nextBit())
        nown = nown->c[1];
      else
        nown = nown->c[0];
      assert(nown != NULL);
    }
    bool sign = bitReader.nextBit();
    int value = nown->value;
    run = value / 100;
    level = (value % 100) * ((sign == 0) ? 1 : -1);
    // epr("decode run level %d %d = %d\n", run, level, value);
  }
};

class BlockTypeTree : public HuffmanTree {
public:
  BlockTypeTree(char *fileName, BitReader &bitReader)
      : HuffmanTree(fileName, bitReader) {
    FILE *f = fopen(fileName, "r");
    root = new node();
    char path[1000];
    int typebool[5];
    while (fscanf(f, "%s %d %d %d %d %d\n", path, &typebool[0], &typebool[1],
                  &typebool[2], &typebool[3], &typebool[4]) == 6) {
      node *nown = root;
      int value = 0, base = 1;
      for (int i = 0; i < 5; i++) {
        if (typebool[i])
          value += base;
        base *= 2;
      }
      for (size_t i = 0; i < strlen(path); i++) {
        if (path[i] == '0')
          nown = nown->getChild(0);
        else if (path[i] == '1')
          nown = nown->getChild(1);
        else
          assert(false);
      }
      nown->setValue(value);
    }
  }
};

class MPEGDecoder {
public:
  char *inputFileName;
  char *timestr;
  int inputfd;

  char *nowStr;
  char *imageContent;
  std::vector<byte> imageContentv;
  int imageContentIndex;
  int imageContentSize;
  int nowIndex;
  int contentIndex;
  BitReader fileReader;

  SequenceHeader sequenceHeader = {};
  GOPHeader GOPheader = {};
  PictureHeader pheader = {};
  SliceHeader sh = {};
  MacroBlockHeader mbh = {};
  BlockHeader bh = {};
  OriginalBlock ob;

  int width, height, precision;

  std::chrono::steady_clock::time_point startTime, previousTime, currentTime;
  BitReader bitReader;
  bool EOI;
  bool isShow;

  void readInputFile();
  void nextStartCode();

  HuffmanTree *motionTree, *Itree, *Ptree, *Btree, *addressTree, *patternTree,
      *luminanceTree, *CrTree, *firstTree, *lastTree;

  bool nextEOF() { return fileReader.EOI; }
  bool nextByte(unsigned char &c) {
    if (nextEOF())
      return false;
    // imageContentIndex >= imageContentSize)
    // c = imageContent[imageContentIndex++];
    // return true;
    c = fileReader.getBits(8);
    return true;
  }
  bool nextBits(int &v, int length) {
    if (nextEOF())
      return false;
    v = fileReader.getBits(length);
    return true;
  }
  bool nextBits(byte &v, int length) {
    if (nextEOF())
      return false;
    v = fileReader.getBits(length);
    return true;
  }
  bool nextBit(bool &v) {
    if (nextEOF())
      return false;
    // if(imageContentIndex >= imageContentSize)
    // return false;
    v = fileReader.getBits(1);
    return true;
  }

  bool nextInt(int &v) {
    if (imageContentIndex >= imageContentSize)
      return false;
    v = 0;
    v += imageContent[imageContentIndex++];
    v <<= 8;
    v += imageContent[imageContentIndex++];
    v <<= 8;
    v += imageContent[imageContentIndex++];
    v <<= 8;
    v += imageContent[imageContentIndex++];
    return true;
  }

  void initTime() {
    getCurrentTime(currentTime);
    getCurrentTime(previousTime);
    getCurrentTime(startTime);
  }
  void getCurrentTime(std::chrono::steady_clock::time_point &timer) {
    timer = std::chrono::steady_clock::now();
  }
  void recordTime(const char *s) {
    getCurrentTime(currentTime);
    fprintf(stderr, "%s, use %lf time\n", s,
            double(std::chrono::duration_cast<std::chrono::milliseconds>(
                       currentTime - previousTime)
                       .count()) /
                1000);
    previousTime = currentTime;
  }

  MPEGDecoder(char *inputs, char *timestr)
      : inputFileName(inputs), timestr(timestr) {
    sequenceHeader.reset();
    GOPheader.reset();
    pheader.reset();
    sh.reset();
    mbh.reset();
    bh.reset();

    EOI = false;
    motionTree = new ValueTree("huffman_tables/motion_vector.txt", fileReader);
    Itree = new BlockTypeTree("huffman_tables/intra_macroblock_type.txt",
                              fileReader);
    Ptree =
        new BlockTypeTree("huffman_tables/p_macroblock_type.txt", fileReader);
    Btree =
        new BlockTypeTree("huffman_tables/b_macroblock_type.txt", fileReader);
    addressTree =
        new ValueTree("huffman_tables/macroblock_addr.txt", fileReader);
    patternTree =
        new ValueTree("huffman_tables/coded_block_pattern.txt", fileReader);
    luminanceTree =
        new ValueTree("huffman_tables/dct_dc_size_luminance.txt", fileReader);
    CrTree =
        new ValueTree("huffman_tables/dct_dc_size_chrominance.txt", fileReader);
    firstTree = new RunLevelTree("huffman_tables/diff_first.txt", fileReader);
    lastTree = new RunLevelTree("huffman_tables/diff_last.txt", fileReader);
    b_buf = new YCbCrBuffer;
    c_buf = new YCbCrBuffer;
    f_buf = new YCbCrBuffer;
  }

  // input
  void videoSequence();
  void readSequenceHeader();
  void groupOfPictures();
  void readGOPHeader();
  void readPicture();
  void readPictureHeader();
  void readSlice();
  void readSliceHeader();
  void readMacroBlock();
  void readBlock(int i);

  byte readStartCode();

  void recoverBlock(int i);
  void writeBlock(int i, int address);
  void addMotionVector(int i);
  void write_skipped_macroblock(int address);
  template <int W>
  void applyMotionCompensation(double pel[8][8], double pel_past[][W],
                               int recon_right, int recon_down, int index,
                               bool half, int macroblock_addr, int mb_width);

  // used in calculate address
  int macroBlock;

  // Others
  void print();

  // reading
  void readHeader();
  int getBitsValue(int len, int val);
  char getBits(int num);

  // main decoding process
  void decode();
  void output(YCbCrBuffer *buf);
  int mbhNum;

private:
  CImgDisplay main_disp;
  YCbCrBuffer *b_buf, *c_buf, *f_buf;

  int readSize();
  int readInt(int len);
  void readSection(char *s, int size);
  void readPart(char *part, int size);
  void ignoreSection();

  bool handleEscape(unsigned char c);
  bool handleEscape_content(unsigned char c);
};
