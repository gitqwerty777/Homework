#include "JPEGDecoder.h"

//Baseline JPEG Decoder
//assumption: all MCU predision = 1 (8x8)
//does not handle some headers

void JPEGDecoder::decode(){
    initTime();
    epr("readHeader\n");
    readHeader();
    recordTime("readHeader");
    epr("recover dc diff\n");
    recoverDCdiff();
    recordTime("recover dc diff");
    epr("dequantify\n");
    dequantify();
    recordTime("dequantify");
    epr("unzagzig\n");
    unzagzig();
    recordTime("unzagzig");
    epr("IDCT\n");//main part of decoding
    IDCT();
    recordTime("IDCT");
    epr("unsubsample\n");
    combineMCU();
    recordTime("unsubsample");
    epr("combine Blocks\n");
    combineBlock();
    recordTime("combine Blocks");
    epr("cut image\n");
    cutImage();
    epr("color conversion\n");
    YCbCrtoRGB();
    recordTime("colorConversion");
    epr("saveResult\n");
    saveResult();
    recordTime("saveResult");
}

void JPEGDecoder::readHeader(){
  FILE* inputf = fopen(inputFileName, "rb");
  assert(inputf);
  inputfd = fileno(inputf);
  struct stat info;
  int result = stat(inputFileName, &info);
  imageContent = new char[info.st_size];
  assert(read(inputfd, imageContent, info.st_size) == info.st_size);//read all in one read()
  imageContentIndex = 0;
  imageContentSize = info.st_size;

  unsigned char c;
  bool isEscape = false;

  while(nextByte(c) == 1){
    if(isEscape){
      isEscape = handleEscape(c);
      epr("handle %d\n", c);
      char escapeStr[100];
      sprintf(escapeStr, "handle escape 0x%02X", (unsigned int)(c & 0xFF));
      recordTime(escapeStr);
    } else if(c == escapeHeader){
      isEscape = true;
      continue;
    }
  }
}

void JPEGDecoder::print(){
  epr("AC Huffman tables\n");
  for(auto& table: AChuffmanTables)
      table.print();
  epr("DC Huffman tables\n");
  for(auto& table: DChuffmanTables)
      table.print();
  epr("Quantiztion tables\n");
  for(auto& table: qtables)
      table.print();
  epr("Color table\n");
  for(auto &it: colorInfos)
      it.second.print();
}

void JPEGDecoder::defineQuanizationTable(){
  /*a)         精度及量化表ID   1字節            高4位：精度，只有兩個可選值
    0：8位；1：16位
    低4位：量化表ID，取值範圍為0～3
    b)        表項       (64×(精度+1))字節              例如8位精度的量化表
    其表項長度為64×（0+1）=64字節
    本標記段中，字段②可以重複出現，表示多個量化表，但最多只能出現4次。
  */
  int size = readSize();
  char s[size];
  readSection(s, size);

  while(nowIndex < size){
    int pid = readInt(1);
    int precision = pid/16;
    int qtableid = pid%16;
    int tableSize = 64*(precision+1);
    std::vector<int> tableValue;
    tableValue.resize(tableSize);
    for(int i = 0; i < tableSize; i++){
      tableValue[i] = readInt(1);
    }

    qtables[qtableid].id = qtableid;
    qtables[qtableid].precision = precision;
    qtables[qtableid].table = tableValue;
    qtables[qtableid].print();
  }
}

inline int JPEGDecoder::readSize(){
    unsigned char s[2];
    nextByte(s[0]);
    nextByte(s[1]);
    int size = (s[1] | s[0] << 8) - 2;//http://stackoverflow.com/a/17071522
  epr("read header section size %d\n", size);
  return size;
}

inline void JPEGDecoder::readSection(char* s, int size){
  memcpy(s, &imageContent[imageContentIndex], size);
  imageContentIndex += size;
  nowIndex = 0; nowStr = s;
}

void JPEGDecoder::ignoreSection(){
  int size = readSize();
  char s[size];
  readSection(s, size);
}

void JPEGDecoder::startOfFrame(){
  int size = readSize();
  char s[size];
  readSection(s, size);
  /*
    ② 精度                 1字節     每個數據樣本的位數
    通常是8位，一般軟件都不支持 12位和16位
    ③ 圖像高度           2字節     圖像高度（單位：像素），如果不支持 DNL 就必須 >0
    ④ 圖像寬度           2字節     圖像寬度（單位：像素），如果不支持 DNL 就必須 >0
    ⑤ 顏色份量數        1字節     只有3個數值可選
    1：灰度圖；3：YCrCb或YIQ；4：CMYK
    而JFIF中使用YCrCb，故這裡顏色份量數恆為3
    ⑥顏色份量信息      顏色份量數×3字節（通常為9字節）
    a)         顏色份量ID                 1字節
    b)        水平/垂直採樣因子      1字節            高4位：水平採樣因子
    低4位：垂直採樣因子
    c)        量化表                         1字節            當前份量使用的量化表的ID
    本標記段中，字段⑥應該重複出現，有多少個顏色份量（字段⑤），就出現多少次（一般為3次）。
   */
  this->precision = readInt(1);
  this->height = readInt(2);
  this->width = readInt(2);
  int colorSize = readInt(1);
  epr("SOF: prec %d height %d width %d colorSize %d\n", this->precision, this->height, this->width, colorSize);
  for(int i = 0; i < colorSize; i++){
    int colorid = readInt(1);
    int colorSample = readInt(1);
    int verticalSample = colorSample%16;
    int horizontalSample = colorSample/16;
    int colorQuant = readInt(1);
    colorInfos[colorid] = ColorInfo(colorid, verticalSample, horizontalSample, colorQuant);
    colorInfos[colorid].print();
  }
}

inline int JPEGDecoder::readInt(int len){
  unsigned char c;
  int ans = 0;
  for(int i = 0; i < len; i++){
      ans <<= 8;
      c = nowStr[nowIndex++];
      ans += c;
  }
  return ans;
}

void JPEGDecoder::defineHuffmanTable(){
  int size = readSize();
  char s[size];
  readSection(s, size);
  /*② 哈夫曼表              數據長度-2字節
    a)       表ID和表類型            1字節            高4位：類型，只有兩個值可選
    0：DC直流；1：AC交流
    低4位：哈夫曼表ID，
    注意，DC表和AC表分開編碼
    b)      不同位數的碼字數量    16字節
    c)      編碼內容       16個不同位數的碼字數量之和（字節）

    本標記段中，字段②可以重複出現（一般4次），也可以致出現1次。*/
  while(nowIndex < size){
    int idtype = readInt(1);
    int tableType = idtype/16;
    int tableid = idtype%16;

    epr("hufftable %s-%d\n", (tableType==TYPEDC)?"DC":"AC", tableid);
    HuffmanTable& huffmanTable = (tableType == TYPEDC)?DChuffmanTables[tableid]:AChuffmanTables[tableid];
    for(int i = 0; i < 16; i++){
      huffmanTable.bitSize[i] = readInt(1);
    }
    huffmanTable.buildSizeTable();
    for(size_t i = 0; i < huffmanTable.huffSize.size(); i++){
        int value = readInt(1);
        huffmanTable.huffValue.push_back(value);
    }
    huffmanTable.buildCodeTable();
    huffmanTable.print();
  }
}

void JPEGDecoder::readDRI(){
    ignoreSection();
  /*
    ②MCU塊的單元中的重新開始間隔
    2字節     設其值為n，則表示每n個MCU塊就有一個
    RSTn標記。第一個標記是RST0，第二個是
    RST1等，RST7後再從RST0重複。
    如果沒有本標記段，或間隔值為0時，就表示不存在重開始間隔和標記RST
   */
}

void JPEGDecoder::startScan(){
  int size = readSize();
  char s[size];
  readSection(s, size);

  int colorSize = readInt(1);
  for(int i = 0; i < colorSize; i++){
    int colorid = readInt(1);
    int acdcid = readInt(1);
    int dcid = acdcid/16;
    int acid = acdcid%16;
    //additional information, base information in SOF
    colorInfos[colorid].colorid = colorid;
    colorInfos[colorid].dcid = dcid;
    colorInfos[colorid].acid = acid;
    colorInfos[colorid].print();
  }
  selectStart = readInt(1);
  selectEnd = readInt(1);
  select = readInt(1);
  epr("select start %d end %d %d\n", selectStart, selectEnd, select);

  scanImage();//read all datas(excluding header)
  epr("image content size %d\n", content.size());

  bitReader.init(content);
  while(!bitReader.EOI)
    readMCU();
}

void JPEGDecoder::scanImage(){
  content.clear();
  contentIndex = 0;
  content.resize(imageContentSize - imageContentIndex);
  epr("scan image\n");
  unsigned char c;
  bool isEscape = false;
  while(nextByte(c)){
    if(isEscape){
      isEscape = handleEscape_content(c);
    } else if(c == escapeHeader){
      isEscape = true;
    } else {
        content[contentIndex] = c;
        contentIndex++;
    }
    if(EOI)
        break;
  }
}

bool JPEGDecoder::handleEscape(unsigned char c){
  switch(c){
  case 0xd8://start of image
    epr("HEADER: SOI\n");
    break;
  case 0xd9://end of image
    epr("HEADER: EOI\n");
    EOI = true;
    break;
  case 0xdb://define quantization table
    epr("HEADER: DQT\n");
    defineQuanizationTable();
    break;
  case 0xdc:
    epr("HEADER: DNL\n");
    readDNL();
    break;
  case 0xc4:
    epr("HEADER: DHT\n");
    defineHuffmanTable();
    break;
  case 0xc8:
      epr("HEADER: JPG\n");
      break;
  case 0xdd://define restart interval
      epr("HEADER: DRI\n");
      readDRI();
      break;
 case 0xde:
     epr("HEADER: DHP\n");
     readDHP();
     break;
 case 0xdf:
     epr("HEADER: EXP\n");
     readEXP();
     break;
  case 0xfe:
      epr("HEADER: COM\n");
      readCOM();
      break;
  case 0xda://start of scan
      epr("HEADER: SOS\n");
      startScan();
      break;
  case 0xff:
      return true;
      break;
  default:
      if(0xe0 <= c && c <= 0xef){//application segment
          epr("HEADER: APP%d\n", (c-0xe0));
          readAPP();
      } else if(0xd0 <= c && c <= 0xd7){//restart with modulo 8 = c
          epr("HEADER: RST%d\n", (char(c)-0xd0));
          readRST();
      } else if(0xc0 <= c && c <= 0xcf){//DCT
          epr("HEADER: SOF%d\n", (c-0xc0));//if == 0, baseline DCT
          startOfFrame();
      } else {
          epr("ERROR: unknown header 0x%x\n", c);
          //assert(1==0);
      }
  }
  return false;
}

bool JPEGDecoder::handleEscape_content(unsigned char c){
/*
    由於在JPEG文件中0xFF具有標誌性的意思，所以在壓縮數據流(真正的圖像信息)中出現0xFF，就需要作特別處理。具體方法是，在數據0xFF後添加一個沒有意義的0x00。換句話說，如果在圖像數據流中遇到0xFF，應該檢測其緊接著的字符，如果是
    1）0x00，則表示0xFF是圖像流的組成部分，需要進行譯碼；
    2）0xD9，則與0xFF組成標記EOI，則圖像流結束，同時圖像文件結束；
    3）0xD0~0xD7,則組成RSTn標記，則要忽視整個RSTn標記，即不對當前0xFF和緊接的0xDn兩個字節進行譯碼，並按RST標記的規則調整譯碼變量；
    3）0xFF，則忽視當前0xFF，對後一個0xFF再作判斷；
    4）其他數值，則忽視當前0xFF，並保留緊接的此數值用於譯碼。
   */
switch(c){
  case 0xd9://end of image
    epr("HEADER: EOI\n");
    EOI = true;
    break;
  case 0x00:
      content[contentIndex] = escapeHeader;
      contentIndex++;

    break;
  case 0xff:
    return true;
    break;
  default:
    if(0xd0 <= c && c <= 0xd7){//restart with modulo 8 = c
        epr("HEADER: RST%d\n", (char(c)-0xd0));//TODO: is it used?
    } else {
        content[contentIndex] = c;
        contentIndex++;
    }
 }
  return false;
}

void JPEGDecoder::recoverDCdiff(){
    int dc_prev[MAX_COMP_NUM] = {0};
    int compNum;
    for (auto& mcu: mcus){
        compNum = 0;
        for(auto& comp: mcu.v){
            for(auto& du: comp){
                du[0] += dc_prev[compNum];
                dc_prev[compNum] = du[0];
            }
            compNum++;
        }
    }
}

void JPEGDecoder::dequantify(){
    for (auto& mcu: mcus){
        int colorCount = 0;
        for(auto it: colorInfos){
            ColorInfo& cinfo = it.second;
            for(auto& vv: mcu.v[colorCount]){
                int tablei = 0;
                for(int &value: vv){
                    value *= qtables[cinfo.qtableid].table[tablei];
                    tablei++;
                }
            }
            colorCount++;
        }
        //mcu.print();
    }
}

void JPEGDecoder::unzagzig(){
    std::vector<int> pre;
    pre.resize(64);
    for (auto& mcu: mcus)
        for(auto& vv: mcu.v)
            for(auto &vvv: vv){
                pre = vvv;
                for(int i = 0; i < 8; i++)
                    for(int j = 0; j < 8; j++)
                        vvv[i*8+j] = pre[zagzigorder[i][j]];
            }
}

void JPEGDecoder::readMCU(){
  epr("read mcu %d\n", mcus.size()+1);
  MCU mcu;
  std::vector<int> colorSampleSize;
  for(auto it: colorInfos)
      colorSampleSize.push_back(it.second.H*it.second.V);
  mcu.setSize(colorSampleSize);
  for(auto it :colorInfos){  //for each colorInfo
    ColorInfo& cinfo = it.second;
    std::vector<int> data;
    data.resize(64);
    for (int j = 0; j < cinfo.H*cinfo.V; j++){//for each subsample
        if(!bitReader.EOI){
            readDataUnit(cinfo.colorid, mcu.v[cinfo.colorid-1][j]);
        }
    }
  }
  if(!bitReader.EOI){
      mcu.index = mcus.size();
      mcu.print();
      mcus.push_back(mcu);
  }
}

inline int JPEGDecoder::getBitsValue(int len, int val){//TODO: know what it is
    //Calculate the value from the "additional" bits in the huffman data
    if(!(val & (1 << (len-1)))){
        val -= (1 << len) -1;
    }
    return val;
}

void JPEGDecoder::readDataUnit(int colorIndex, std::vector<int>& data){
    data.clear();
    data.reserve(64);
    ColorInfo& cinfo = colorInfos[colorIndex];
    HuffmanTable htable = DChuffmanTables[cinfo.dcid];

    while(data.size() < 64){  //Fill data with 64 coefficients
    int code = 0;
    int value = -1;
    for(int bits = 1; bits <= 16; bits++){
        code <<= 1;
        bool val = bitReader.nextBit();
        if(bitReader.EOI)
            break;
        code |= val;
        if(htable.exist(bits, code)){//fit length and codeword
            value = htable.codeEntry[bits][code];
            break;
        }
    }
    if(value == -1){
        epr("code not found\n");//also means end of reading?
        break;
    }
    //only one DC to read
    htable = AChuffmanTables[cinfo.acid];
    if(value == 0xF0){//TODO:? If ZRL fill with 16 zero coefficients
      for(int i = 0; i < 16; i++)
        data.push_back(0);
      continue;
    }
    //TODO: know
    if(data.size() != 0){
        if(value == 0x00){// End of block
            int originalDataSize = data.size();
            data.resize(64);
            //Fill the rest of the DU with zeros
            std::fill(data.begin()+originalDataSize, data.end(), 0);
            break;
        }
      //The first part of the AC value
      //is the number of leading zeros
      for(int i = 0; i < (value >> 4); i++){
        if(data.size() < 64)
          data.push_back(0);
      }
      value &= 0x0F;
    }
    if(data.size() >= 64)
        break;
    if(value != 0){
        //The rest of value is the amount of "additional" bits
        int val = bitReader.getBits(value);
        //epr("ac: %d\n", val);
        if(val == BIT_END)
            break;
        int num = getBitsValue(value, val);//Decode the additional bits
        data.push_back(num);
    } else {
        data.push_back(0);
    }
  }
  if(data.size() != 64)
      epr("MCU: reading wrong size %d\n", data.size());
}

inline double ccc(int x){
    if(x==0)
        return 1.0/std::sqrt(2.0);
    else
        return 1.0;
}
inline int rounding(double x){
    if(x > 0.0)
        return int(floor(x+0.5));
    else
        return int(ceil(x-0.5));
}

double c(int i) {
    static double x = 1.0/sqrt(2.0);
    if (i == 0) {
        return x;
    } else {
        return 1.0;
    }
}

void JPEGDecoder::IDCT(){
    //init lookup table to speed up
    double idct_table[8][8];
    for(int i = 0; i < 8; i++)
        for(int j = 0; j < 8; j++)
            idct_table[i][j] = ccc(i)*std::cos(((2.0*j+1.0)*double(i)*M_PI)/16.0)/2.0;
    #ifdef DEBUG
    epr("IDCT table\n");
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            epr("%lf ", idct_table[i][j]);
        }
        epr("\n");
    }
    #endif
    int compIndex, blockIndex;
    for (auto& mcu: mcus){
        compIndex = 0;
        for(auto& vv: mcu.v){
            blockIndex = 0;
            for(auto &vvv: vv){
                double out[8][8] = {{0}};
                for(int j = 0; j < 8; j++)
                    for(int i = 0; i < 8; i++)
                        for(int u = 0; u < 8; u++)
                            for(int v = 0; v < 8; v++)
                                out[j][i] += vvv[v*8+u]*idct_table[u][i]*idct_table[v][j];
                for(int u = 0; u < 8; u++)
                    for(int v = 0; v < 8; v++){
                        mcu.dv[compIndex][blockIndex][u][v] = out[u][v];
                    }
                /*epr("after idct %d (%d, %d)\n", mcu.index, compIndex, blockIndex);
                for(int u = 0; u < 8; u++){
                    for(int v = 0; v < 8; v++)
                        epr("%lf ", mcu.dv[compIndex][blockIndex][u][v]);
                    epr("\n");
                    }*/
                blockIndex++;
            }
            compIndex++;
        }
    }
}

void JPEGDecoder::readAPP(){//ignore
    ignoreSection();
}
void JPEGDecoder::readRST(){//ignore
    ignoreSection();
}
void JPEGDecoder::readDNL(){//ignore
    ignoreSection();
}
void JPEGDecoder::readDHP(){//ignore
    ignoreSection();
}
void JPEGDecoder::readEXP(){//ignore
    ignoreSection();
}
void JPEGDecoder::readCOM(){//ignore
    ignoreSection();
}

void JPEGDecoder::unSubSampling(MCU& mcu){
    mcu.newv.resize(maxV*8);
    for(auto& vv: mcu.newv)
        vv.resize(maxH*8);

    for(int i = 0; i < colorInfos.size(); i++){//for color i
        int Hs= maxH/H[i];//sampled 1 grid = unsampled Hs grid
        int Vs= maxV/V[i];
        int vblocksize = maxV*8/V[i];//unsampled 1 block size, maybe 8 or 16
        int hblocksize = maxH*8/H[i];
        epr("before unSubSampling\n");
        for(auto& vv: mcu.dv[i]){
            for(auto& vvv: vv){
                for(auto& value: vvv)
                    epr("%d ", int(value));
                epr("\n");
            }
            epr("\n");
        }
        for(int v = 0; v < maxV*8; v++)
            for(int h = 0; h < maxH*8; h++){
                int blockv = v/vblocksize;//in ? th sampled block
                int blockh = h/hblocksize;
                mcu.newv[v][h].push_back(mcu.dv[i][blockv*H[i]+blockh][(v/Vs)%8][(h/Hs)%8]);
            }
        epr("final unsample componenet\n");
        for(int v = 0; v < maxV*8; v++){
            for(int h = 0; h < maxH*8; h++)
                epr("%d ", int(mcu.newv[v][h][i]));
            epr("\n");
        }
    }
}

void JPEGDecoder::combineMCU(){
    maxH = maxV = 0;
    for(auto it: colorInfos){
        ColorInfo& cinfo = it.second;
        if(cinfo.H > maxH)
            maxH = cinfo.H;
        if(cinfo.V > maxV)
            maxV = cinfo.V;
        H.push_back(cinfo.H);
        V.push_back(cinfo.V);
    }
    for(auto& mcu: mcus){
       unSubSampling(mcu);
    }
}

void JPEGDecoder::combineBlock(){
    original.resize(height+64);//prevent block exceed boundary
    for(auto& v: original)
        v.resize(width+64);
    int offsetw = 0;
    int offseth = 0;
    int maxh, maxw;
    epr("total %d mcus\n", mcus.size());
    for(MCU& mcu: mcus){
        maxh = mcu.newv.size();
        for(int h = 0; h < maxh; h++){
            maxw= mcu.newv[h].size();
            for(int w = 0; w < maxw; w++){
                original[h+offseth][w+offsetw] = ColorNode(mcu.newv[h][w]);
            }
        }
        epr("maxh %d maxw %d\n", maxh, maxw);
        //goto next block
        offsetw += maxw;
        if(offsetw >= width){
            offsetw = 0;
            offseth += maxh;
        }
    }
    /*int count = 0;
    for(auto& v: original){
        epr("block: row%d\n", count++);
        for(auto& vv: v){
            vv.print();
            epr(", ");
        }
        epr("\n");
        }*/
}

inline void MCU::printFinal(){
    for(auto& v: final)
        for(auto& vv: v)
            vv.print();
}

inline void JPEGDecoder::cutImage(){
    original.resize(height);
    for(auto& v: original)
        v.resize(width);
}

inline void JPEGDecoder::YCbCrtoRGB(){
    for(auto& v: original)
        for(ColorNode& cn: v)
            cn.YCbCrtoRGB();
}

inline int fit(double x) {
    if (x > 255.0) {
        return 255;
    } else if (x < 0.0) {
        return 0;
    } else {
        return rounding(x);
    }
}
inline void ColorNode::YCbCrtoRGB(){
    double Y = v[0];
    double Cb = v[1];
    double Cr = v[2];
    v[0] = fit(Y + 1.402*Cr + 128);
    v[1] = fit(Y - 0.34414*Cb - 0.71414*Cr + 128);
    v[2] = fit(Y + 1.772*Cb + 128);
}

void JPEGDecoder::saveResult(){
    FILE* f, *ff;
    if(isShow){
        f = fopen("midoutput.raw","w");//tk version image
        for(auto& v: original)
            for(auto& cn: v)
                fprintf(f, "#%02x%02x%02x\n", int(cn.v[0]), int(cn.v[1]), int(cn.v[2]));
        fclose(f);
        return;
    }
    f = fopen("rgb.raw","wb");//PIL rgb image
    int fd = fileno(f);
    unsigned char* bin = new unsigned char[sizeof(unsigned char)*width*height*3];
    int bincount = 0;
    for(auto& v: original)
        for(auto& cn: v)
            for(auto value: cn.v)
                bin[bincount++] = value;
    write(fd, bin, bincount);
    fclose(f);
    delete[] bin;
    #ifdef DEBUG
    ff = fopen("midoutput.rgb","w");//text-form RGB
    for(auto& v: original)
        for(auto& cn: v)
            fprintf(ff, "%d %d %d\n", int(cn.v[0]), int(cn.v[1]), int(cn.v[2]));
    fclose(ff);
    #endif
    ff = fopen("size.txt", "w");
    fprintf(ff, "%d\n%d", height, width);
    fclose(ff);
    ff = fopen("outputFileName", "w");
    fprintf(ff, "%s", outputFileName);
    fclose(ff);
}
