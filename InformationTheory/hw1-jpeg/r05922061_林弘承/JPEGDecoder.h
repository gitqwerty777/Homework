#pragma once

#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cstring>
#include <cmath>
#include <vector>
#include <map>
#include <chrono>
#include <unistd.h>
#include <sys/stat.h>
//#define DEBUG
//SPEEDUP: int -> char, no copy vector

//max color number
#define MAX_COMP_NUM 4
const int TYPEDC = 0;
const int TYPEAC = 1;
const int escapeHeader = 0xff;
const int BIT_END = 10000;
const int zagzigorder[][8] = {{ 0,  1,  5,  6, 14, 15, 27, 28},
                              { 2,  4,  7, 13, 16, 26, 29, 42},
                              { 3,  8, 12, 17, 25, 30, 41, 43},
                              { 9, 11, 18, 24, 31, 40, 44, 53},
                              {10, 19, 23, 32, 39, 45, 52, 54},
                              {20, 22, 33, 38, 46, 51, 55, 60},
                              {21, 34, 37, 47, 50, 56, 59, 61},
                              {35, 36, 48, 49, 57, 58, 62, 63}};

#ifdef DEBUG
template<typename ...Args>
inline void epr(const char* s, Args... args){
    fprintf(stderr, s, args...);
}
#else
template<typename ...Args>
inline void epr(const char* s, Args... args){
    // do nothing
}
#endif

struct ColorInfo{//one of YCbCr or one of RGB ...
  int colorid;
  int qtableid;
  int dcid, acid;
  int V, H;
  ColorInfo(){}
ColorInfo(int colorid, int V, int H, int qtableid): colorid(colorid), V(V), H(H), qtableid(qtableid) {}
  void print(){
      epr("colorinfo: color id %d vert %d hori %d quantableid %d dcid %d acid %d\n", colorid, V, H, qtableid, dcid, acid);
  }
};

struct ColorNode{
    std::vector<double> v;
    ColorNode(){v.resize(3);}
    ColorNode(std::vector<double>& vv){
        v = vv;
    }
    void print(){
        if(v[0] == 0 && v[1] == 0 && v[2] == 0)
            return;
        epr("(%lf,%lf,%lf)", v[0], v[1], v[2]);
    }
    void reset(){
        v[0] = v[1] = v[2] = 0;
    }
    void YCbCrtoRGB();
};

class HuffmanTable{//TODO: move to new file
 public:
  int bitSize[16];
  std::vector<int> huffSize;
  std::vector<int> huffValue;
  std::vector<std::map<int, int> > codeEntry;//size, code, value

  HuffmanTable(){
      codeEntry.resize(17);//1~16
      huffSize.clear();
  }

  bool exist(int size, int code){
      return codeEntry[size].count(code) > 0;
  }

  void buildSizeTable(){
    for(int i = 1; i <= 16; i++)
      for(int j = 0; j < bitSize[i-1]; j++)
        huffSize.push_back(i);
  }

  void buildCodeTable(){//by JPEG spec
    int k = 0, code = 0;
    int si = huffSize[0];
    while(true){
        if(k >= huffSize.size())
            break;
        codeEntry[huffSize[k]][code] = huffValue[k];
        code++; k++;
        if(huffSize[k] != si){
            if(huffSize[k] == 0){
                break;
            } else {
                while(true){
                    code <<= 1;
                    si++;
                    if(huffSize[k] == si)
                        break;
                }
            }
        }
    }
  }
  void print(){
      for(int i = 1; i <= 16; i++)
          for(auto it: codeEntry[i])
              epr("size %d code%d value %d\n", i, it.first, it.second);
  }
};

struct QuantizationTable{
    int precision;
    int id;
    std::vector<int> table;
    void print(){
        epr("QuantizationTable: id %d preci %d\n", id, precision);
        for(int i = 0; i < 8; i++){
            for(int j = 0; j < 8; j++)
                epr("%d ", table[i*8+j]);
            epr("\n");
        }
    }
};

class MCU{
 public:
    std::vector<std::vector<std::vector<int> > > v;
    double dv[3][4][8][8];
    std::vector<std::vector<std::vector<double> > > newv;
    std::vector<std::vector<ColorNode> > final;
    int index;
  MCU(){}
 MCU(std::vector<std::vector<std::vector<int> > >& v): v(v){
  }
  void printFinal();
  void setSize(std::vector<int> & sizes){
      v.resize(sizes.size());
      int sizeIndex = 0;
      for(int size: sizes)
          v[sizeIndex++].resize(size);
  }
  void print(){
      epr("MCU id %d\n", index);
      for(auto& vv: v){
          for(auto& vvv: vv){
              for(auto i: vvv){
                  epr("%d, ", i);
              }
          }
      }
      epr("\n");
  }
};

class BitReader{
 public:
  std::vector<unsigned char> content;
  int byteIndex;
  int bitIndex;
  unsigned char nowByte;
  bool bits[8];
  bool EOI;

  void seperateBits(){
      int index = 7;
      while(index >= 0){
          bits[index--] = nowByte % 2;
          nowByte>>=1;
      }
  }
  void init(std::vector<unsigned char>& content){//TODO: content
    this->content = content;
    bitIndex = -1;
    byteIndex = 0;
    nowByte = content[byteIndex];
    seperateBits();
    EOI = false;
  }
  bool nextBit(){//TODO: pre-seperate to bit
    if(EOI)
      return 0;
    bitIndex++;
    if(bitIndex >= 8){//0~7
      bitIndex = 0;
      byteIndex++;
      if(byteIndex >= content.size()){
        EOI = true;
        return 0;
      }
      nowByte = content[byteIndex];
      seperateBits();
    }
    //epr("now byte %d btyeindex %d bitindex %d bit %d\n", nowByte, byteIndex, bitIndex, (nowByte>>(7-bitIndex))%2);
    return (bits[bitIndex]);//TODO: speedup
  }
  int getBits(int len){
    int sum = 0;
    for(int i = 0; i < len; i++){
      sum <<= 1;
      int val = nextBit();
      if(val == BIT_END){//impossible
          assert(1==0);
          return BIT_END;
      }
      sum += val;
    }
    return sum;
  }
};

class JPEGDecoder{
 public:
  char* inputFileName;
  char* outputFileName;
  int inputfd;

  //TODO: more readers
  char* nowStr;
  char* imageContent;
  int imageContentIndex;
  int imageContentSize;
  int nowIndex;
  int contentIndex;

  //at most 4 table
  std::vector<HuffmanTable> DChuffmanTables;
  std::vector<HuffmanTable> AChuffmanTables;
  std::vector<QuantizationTable> qtables;
  std::map<int, ColorInfo> colorInfos;
  std::vector<MCU> mcus;
  std::vector<unsigned char>& content = bitReader.content;//TODO: remove
  std::vector<std::vector<ColorNode> > original;

  std::vector<int> H;
  std::vector<int> V;
  int maxH, maxV;
  int width, height, precision;
  int selectStart, selectEnd, select;

  std::chrono::steady_clock::time_point startTime, previousTime, currentTime;
  BitReader bitReader;
  bool EOI;
  bool isShow;

  bool nextByte(unsigned char& c){
      if(imageContentIndex >= imageContentSize)
          return false;
      c = imageContent[imageContentIndex++];
      return true;
  }
  void initTime(){
      getCurrentTime(currentTime);
      getCurrentTime(previousTime);
      getCurrentTime(startTime);
  }
  void getCurrentTime(std::chrono::steady_clock::time_point & timer){
      timer = std::chrono::steady_clock::now();
  }
  void recordTime(const char* s){
      getCurrentTime(currentTime);
      fprintf(stderr, "%s, use %lf time\n", s, double(std::chrono::duration_cast<std::chrono::milliseconds>(currentTime-previousTime).count())/1000);
      previousTime = currentTime;
  }

JPEGDecoder(char* inputs, char* outputs, bool isShow): inputFileName(inputs), outputFileName(outputs), isShow(isShow){
      EOI = false;
      DChuffmanTables.resize(4);
      AChuffmanTables.resize(4);
      qtables.resize(4);
  }

  //Others
  void print();
  void saveResult();

  //reading
  void readHeader();
  int getBitsValue(int len, int val);
  char getBits(int num);

  //reading MCU
  void readMCU();
  void readDataUnit(int index, std::vector<int>& data);

  //main decoding process
  void decode();
  void recoverDCdiff();
  void dequantify();
  void IDCT();
  void unzagzig();
  void combineMCU();
  void combineBlock();
  void unSubSampling(MCU& mcu);
  void cutImage();
  void YCbCrtoRGB();

  //headers
  void readDNL();
  void readDRI();
  void readDHP();
  void readEXP();
  void readCOM();
  void readAPP();
  void readRST();
  void startOfFrame();
  void defineQuanizationTable();
  void defineHuffmanTable();
  void startScan();
  void scanImage();

private:
  int readSize();
  int readInt(int len);
  void readSection(char* s, int size);
  void readPart(char* part, int size);
  void ignoreSection();

  bool handleEscape(unsigned char c);
  bool handleEscape_content(unsigned char c);
};
