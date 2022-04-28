#pragma once

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

const int BIT_END = 2147483647;
typedef unsigned char byte; // uimsbf and bit <= 8
typedef unsigned int uint;  // uimsbf
// bslbf = int?

//#define DEBUG
#ifdef DEBUG
template <typename... Args> inline void epr(const char *s, Args... args) {
  fprintf(stderr, s, args...);
}
#else
template <typename... Args> inline void epr(const char *s, Args... args) {
  // do nothing
}
#endif

const char start_code[] = "000000000000000000000001";
const char macroblock_stuffing[] = "00000001111";
const char macroblock_escape[] = "00000001000";
const char picture_start_code[] = "00000000000000000000000100000000";
const char user_data_start_code[] = "00000000000000000000000110110010";
const char extension_start_code[] = "00000000000000000000000110110101";
const char group_start_code[] = "00000000000000000000000110111000";
const char sequence_start_code[] = "00000000000000000000000110110011";
const char sequence_end_code[] = "00000000000000000000000110110111";

inline bool inSlice(int startCode) {
  return (startCode >= 256 + 1 &&
          startCode <= 256 + 175); // 256=startcode, 1=1, 175=AF
}

class BitReader {
public:
  std::vector<unsigned char> content;
  BitReader *copyb;
  bool EOI;

  void seperateBits() {
    int index = 7;
    while (index >= 0) {
      bits[index--] = nowByte % 2;
      nowByte >>= 1;
    }
  }
  void
  init(std::vector<unsigned char> &content) { // TODO: content directly assign
    copyb = new BitReader;
    this->content = content;
    bitIndex = -1;
    byteIndex = 0;
    nowByte = content[byteIndex];
    seperateBits();
    EOI = false;
  }
  bool nextBit() {
    if (EOI)
      return 0;
    bitIndex++;
    if (bitIndex >= 8) { // 0~7
      byteIndex++;
      bitIndex = 0;
      if (byteIndex >= content.size()) {
        fprintf(stderr, "EOI\n");
        EOI = true;
        return 0;
      }
      nowByte = content[byteIndex];
      seperateBits();
      nowByte = content[byteIndex];
    }
    // fprintf(stderr, "now byte %d btyeindex %d bitindex %d bit %d\n", nowByte,
    // byteIndex, bitIndex, bits[bitIndex]);
    return (bits[bitIndex]); // TODO: speedup
  }
  void discard(int len) {
    for (int i = 0; i < len; i++) {
      int val = nextBit();
      if (val == BIT_END) { // impossible
        assert(1 == 0);
      }
    }
  }
  bool checkBit(bool b) {
    copyb->copyProgress(*this);
    bool readb;
    readb = nextBit();
    this->copyProgress(*copyb);
    return (readb == b);
  }
  int top() {
    copyb->copyProgress(*this);
    int v;
    v = getBits(32);
    this->copyProgress(*copyb);
    return v;
  }
  int getBit() { return getBits(1); }
  void checkAndDiscard(const char s[]) { // check and read
    bool readb;
    int len = strlen(s);
    int nowi = 0;
    while (nowi < len) {
      readb = nextBit();
      if (readb != (s[nowi] == '1' ? true : false)) {
        assert(false);
      }
      nowi++;
    }
  }
  bool checkBits(const char s[]) { // only check, no read
    bool isSame = true;
    copyb->copyProgress(*this);
    bool readb;
    int len = strlen(s);
    int nowi = 0;
    while (nowi < len) {
      readb = nextBit();
      if (readb != (s[nowi] == '1' ? true : false)) {
        isSame = false;
        break;
      }
      nowi++;
    }
    this->copyProgress(*copyb);
    return isSame;
  }
  bool byteAligned() {
    return bitIndex == 7 ||
           bitIndex == -1; // on the boundary, next bit is first bit of byte
  }
  void copyProgress(BitReader &b) {
    EOI = b.EOI;
    byteIndex = b.byteIndex;
    bitIndex = b.bitIndex;
    nowByte = b.nowByte;
    memcpy(bits, b.bits, sizeof(bool) * 8);
  }
  int getBits(int len) {
    int sum = 0;
    for (int i = 0; i < len; i++) {
      sum <<= 1;
      int val = nextBit();
      if (val == BIT_END) { // impossible
        assert(1 == 0);
        return BIT_END;
      }
      sum += val;
    }
    return sum;
  }
  void print() {
    // fprintf(stderr, "byteindex %d, bitindex %d nowbyte %d\n", byteIndex,
    // bitIndex, nowByte);
  }
  bool checkSlice() {
    bool isSame = true;
    copyb->copyProgress(*this);
    bool readb;
    int nowi = 0;
    int startCode = getBits(32);
    if (!inSlice(startCode))
      isSame = false;
    this->copyProgress(*copyb);
    return isSame;
  }
  byte nextByte() { return getBits(8); }
  void nextStartCode() {
    while (!byteAligned())
      assert(!nextBit()); // zerobit
    while (!checkBits(start_code)) {
      byte b = nextByte();
      ;
      assert(b == 0); // zerobyte
    }
  }
  void readExtension() {
    nextStartCode();
    if (checkBits(extension_start_code)) {
      discard(32);
      while (!checkBits(start_code)) {
        discard(8); //_extension_data;
        epr("extension: next bit != 24*0 + 1\n");
      }
      nextStartCode();
      epr("current top is %d\n", top());
    }
    if (checkBits(user_data_start_code)) {
      discard(32);
      while (!checkBits(start_code)) {
        discard(8); // user_data();
        epr("extension: next bit != 24*0 + 1\n");
      }
      nextStartCode();
      epr("current top is %d\n", top());
    }
  }

private:
  int byteIndex;
  int bitIndex;
  byte nowByte;
  bool bits[8];
};
