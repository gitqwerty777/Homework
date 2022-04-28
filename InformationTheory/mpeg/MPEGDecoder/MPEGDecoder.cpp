#include "MPEGDecoder.h"
#include <cmath>
#include <cstring>
#include <ctime>
// http://dvd.sourceforge.net/dvdinfo/mpeghdrs.html

void MPEGDecoder::decode() { // realtime decode: not need to read all files at
                             // first
  initTime();
  epr("readHeader\n");
  readHeader();
}

void MPEGDecoder::readHeader() {
  readInputFile();
  videoSequence();
}

void MPEGDecoder::readInputFile() {
  FILE *inputf = fopen(inputFileName, "rb");
  assert(inputf);
  inputfd = fileno(inputf);
  struct stat info;
  stat(inputFileName, &info);
  imageContentv.resize(info.st_size);
  char *imageContentp = new char[info.st_size];
  assert(read(inputfd, imageContentp, info.st_size) ==
         info.st_size); // read all files in one read()
  imageContentv.assign((unsigned char *)(imageContentp),
                       (unsigned char *)(imageContentp) + info.st_size);
  imageContentIndex = 0;
  imageContentSize = info.st_size;
  epr("read video file size %d\n", imageContentSize);
  fileReader.init(imageContentv);
}

void MPEGDecoder::videoSequence() {
  nextStartCode();
  do {
    readSequenceHeader();
    do {
      groupOfPictures();
    } while (fileReader.checkBits(group_start_code));
  } while (fileReader.checkBits(sequence_start_code));
  epr("output bbuf\n");
  output(b_buf); // last backward frame
  assert(fileReader.checkBits(sequence_end_code));
  fileReader.discard(32);
}

void MPEGDecoder::readSequenceHeader() {
  sequenceHeader.read(&fileReader);
  ob.macroBlockHeight =
      std::ceil(float(sequenceHeader.v) / 16); // row num of macroblock
  ob.macroBlockWidth =
      std::ceil(float(sequenceHeader.h) / 16); // column num of macroblock
}

void MPEGDecoder::groupOfPictures() {
  readGOPHeader();
  nextStartCode();
  do {
    readPicture();
  } while (fileReader.checkBits(picture_start_code));
}

void MPEGDecoder::readGOPHeader() { GOPheader.read(&fileReader); }

void MPEGDecoder::readPicture() {
  readPictureHeader();
  do {
    readSlice();
  } while (fileReader.checkSlice());
  /* 3-Frame Buffers Algorithm - after decode*/
  if (pheader.isI() || pheader.isP()) {
    std::swap(c_buf, b_buf);
  } else {
    epr("output cbuf\n");
    output(c_buf);
  }
}

void MPEGDecoder::readPictureHeader() {
  pheader.read(&fileReader);
  /* 3-Frame Buffers Algorithm - before decode*/
  if (pheader.isI() || pheader.isP()) {
    epr("output fbuf\n");
    std::swap(f_buf, b_buf);
    output(f_buf);
  }
}

void MPEGDecoder::readSlice() {
  readSliceHeader();
  mbhNum = 0;
  do {
    readMacroBlock();
    mbhNum++;
  } while (!fileReader.checkBits("00000000000000000000000"));
  nextStartCode();
}

void MPEGDecoder::readSliceHeader() {
  sh.read(&fileReader);
  ob.slice_vert_pos = sh.vertical_position;
  ob.reset();
}

/*
 If the current macroblock is the first macroblock in the slice, or if the last
 macroblock that was decoded contained no motion vector information (either
 because it was skipped or macroblock_motion_forward was zero),  then
 recon_right_for_prev and recon_down_for_prev shall be set to zero
 */

void MPEGDecoder::write_skipped_macroblock(int address) {
  if (pheader.isP()) {
    ob.recon_right_for = ob.recon_down_for = 0;
    ob.recon_right_back = ob.recon_down_back = 0;
    ob.resetRecon();
  }

  assert(!pheader.isI());

  bool has_f = mbh.isMotionForward;
  bool has_b = mbh.isMotionBackward;
  bool half = has_f && has_b;

  //Y
  for (int y = 0; y < 4; ++y) {
    for (int i = 0; i < 8; ++i) // dct_recon[m][n] = 0 for all m, n in skipped
                                // macroblocks and when pattern[i] == 0
      for (int j = 0; j < 8; ++j)
        ob.recon[i][j] = 0;
    if (has_f)
      applyMotionCompensation(ob.recon, f_buf->y, ob.recon_right_for,
                              ob.recon_down_for, y, half, address,
                              ob.macroBlockWidth);
    if (has_b)
      applyMotionCompensation(ob.recon, b_buf->y, ob.recon_right_back,
                              ob.recon_down_back, y, half, address,
                              ob.macroBlockWidth);
    writeBlock(y, address);
  }
  //Cb
    for (int i = 0; i < 8; ++i)
      for (int j = 0; j < 8; ++j)
        ob.recon[i][j] = 0;
    if (has_f)
      applyMotionCompensation(ob.recon, f_buf->cb, ob.recon_right_for,
                              ob.recon_down_for, 4, half, address,
                              ob.macroBlockWidth);
    if (has_b)
      applyMotionCompensation(ob.recon, b_buf->cb, ob.recon_right_back,
                              ob.recon_down_back, 4, half, address,
                              ob.macroBlockWidth);
    writeBlock(4, address);
    //Cr
    for (int i = 0; i < 8; ++i)
      for (int j = 0; j < 8; ++j)
        ob.recon[i][j] = 0;
    if (has_f)
      applyMotionCompensation(ob.recon, f_buf->cr, ob.recon_right_for,
                              ob.recon_down_for, 5, half, address,
                              ob.macroBlockWidth);
    if (has_b)
      applyMotionCompensation(ob.recon, b_buf->cr, ob.recon_right_back,
                              ob.recon_down_back, 5, half, address,
                              ob.macroBlockWidth);
    writeBlock(5, address);
}

void MPEGDecoder::readMacroBlock() { // hard to read in structure
  int stuffing;                      // 11
  int escape;                        // 11
  while (fileReader.checkBits(macroblock_stuffing)) {
    fileReader.discard(11);
  }
  mbh.address_increment = 0;
  while (fileReader.checkBits(macroblock_escape)) {
    fileReader.discard(11);
    mbh.address_increment +=
        33; // The maximum value of macroblock address increment is 33
  }
  int base = 0;
  addressTree->getValue(base);
  mbh.address_increment += base;

  for (int i = 1; i < mbh.address_increment; ++i)
    write_skipped_macroblock(ob.macroblock_addr + i);

  ob.macroblock_addr += mbh.address_increment;

  switch (pheader.type) {
  case 1:
    Itree->getValue(mbh.type);
    break;
  case 2:
    Ptree->getValue(mbh.type);
    break;
  case 3:
    Btree->getValue(mbh.type);
    break;
  case 4:
    epr("D block\n");
    break;
  }

  mbh.decodeProperties();
  if (mbh.isQuant) {
    nextBits(sh.quantizer_scale, 5);
    epr("new quantizer_scale %d\n", sh.quantizer_scale);
  }

  if (!mbh.isIntra || ob.macroblock_addr - ob.past_intra_addr > 1) {
    ob.dct_dc_y_past = ob.dct_dc_cb_past = ob.dct_dc_cr_past = 1024;
  }
  if (pheader.isB() && mbh.isIntra) {
    ob.recon_right_for_prev = ob.recon_down_for_prev =
        ob.recon_right_back_prev = ob.recon_down_for_prev = 0;
  }

  mbh.motion.reset();
  if (mbh.isMotionForward) { //檢查數字
    motionTree->getValue(mbh.motion.horizontal_forward_code);
    if ((pheader.forward_f != 1) && (mbh.motion.horizontal_forward_code != 0))
      nextBits(mbh.motion.horizontal_forward_r, pheader.forward_r_size);

    motionTree->getValue(mbh.motion.vertical_forward_code);
    if ((pheader.forward_f != 1) && (mbh.motion.vertical_forward_code != 0))
      nextBits(mbh.motion.vertical_forward_r, pheader.forward_r_size);

    ob.recoverMotionVector(
        pheader.forward_f, mbh.motion.horizontal_forward_code,
        mbh.motion.horizontal_forward_r, ob.recon_right_for_prev,
        pheader.forward_vector, ob.recon_right_for);
    ob.recoverMotionVector(pheader.forward_f, mbh.motion.vertical_forward_code,
                           mbh.motion.vertical_forward_r,
                           ob.recon_down_for_prev, pheader.forward_vector,
                           ob.recon_down_for);
  } else {
    if (pheader.isP()) {
      ob.recon_right_for_prev = ob.recon_down_for_prev = 0;
      ob.recon_right_for = ob.recon_down_for = 0; // reset all forward
      mbh.isMotionForward = true;
    } else if (pheader.isB()) { // needed?
      ob.recon_right_for = ob.recon_right_for_prev;
      ob.recon_down_for = ob.recon_down_for_prev;
    }
  }

  if (mbh.isMotionBackward) {
    motionTree->getValue(mbh.motion.horizontal_backward_code);
    if ((pheader.backward_f != 1) && (mbh.motion.horizontal_backward_code != 0))
      nextBits(mbh.motion.horizontal_backward_r, pheader.backward_r_size);

    motionTree->getValue(mbh.motion.vertical_backward_code);
    if ((pheader.backward_f != 1) && (mbh.motion.vertical_backward_code != 0))
      nextBits(mbh.motion.vertical_backward_r, pheader.backward_r_size);
    ob.recoverMotionVector(
        pheader.backward_f, mbh.motion.horizontal_backward_code,
        mbh.motion.horizontal_backward_r, ob.recon_right_back_prev,
        pheader.backward_vector, ob.recon_right_back);
    ob.recoverMotionVector(
        pheader.backward_f, mbh.motion.vertical_backward_code,
        mbh.motion.vertical_backward_r, ob.recon_down_back_prev,
        pheader.backward_vector, ob.recon_down_back);
  }

  mbh.coded_block_pattern = 0;
  if (mbh.isPattern) { // read pattern code
    patternTree->getValue(mbh.coded_block_pattern);
  } else if (mbh.isIntra) {
    ob.resetRecon();
  }
  mbh.print(mbhNum);
  for (int i = 0; i < 6; i++) {
    if ((mbh.coded_block_pattern & (1 << (5 - i))) || mbh.isIntra) {
      readBlock(i);
      recoverBlock(i);
      bh.print();
    } else { // set to 0
      for (int j = 0; j < 8; ++j)
        for (int k = 0; k < 8; ++k)
          ob.recon[j][k] = 0;
    }
    if (!mbh.isIntra) {
      addMotionVector(i);
    }
    writeBlock(i, ob.macroblock_addr);
  }
  if (pheader.isD()) { // D frame?
    fileReader.checkAndDiscard("1");
    epr("[EOF]\n");
  }
  if (mbh.isIntra)
    ob.past_intra_addr = ob.macroblock_addr;
}

void MPEGDecoder::readBlock(int i) {
  memset(bh.dct_zz, 0,
         sizeof(bh.dct_zz)); // dct_zz[i], i>0 shall be set to zero initially
  int index = 0;
  if (mbh.isIntra) {
    if (i < 4) {
      luminanceTree->getValue(bh.dct_dc_size_luminance);
      if (bh.dct_dc_size_luminance != 0) {
        nextBits(bh.dct_dc_differential, bh.dct_dc_size_luminance);
        if (bh.dct_dc_differential & (1 << (bh.dct_dc_size_luminance - 1)))
          bh.dct_zz[0] = bh.dct_dc_differential;
        else
          bh.dct_zz[0] =
              (-1 << (bh.dct_dc_size_luminance)) | (bh.dct_dc_differential + 1);
      } else {
        bh.dct_zz[0] = 0;
      }
    } else {
      CrTree->getValue(bh.dct_dc_size_chrominance);
      if (bh.dct_dc_size_chrominance != 0) {
        nextBits(bh.dct_dc_differential, bh.dct_dc_size_chrominance);
        if (bh.dct_dc_differential & (1 << (bh.dct_dc_size_chrominance - 1)))
          bh.dct_zz[0] = bh.dct_dc_differential;
        else
          bh.dct_zz[0] = (-1 << (bh.dct_dc_size_chrominance)) |
                         (bh.dct_dc_differential + 1);
      } else
        bh.dct_zz[0] = 0;
    }
  } else { // dct_coeff_first
    int run, level;
    firstTree->getValue(run, level);
    index = run;
    epr("dct_coeff_first : run %d, level %d\n", run, level);
    bh.dct_zz[index] = level;
  }
  if (!pheader.isD()) {
    while (!fileReader.checkBits("10")) { // dct_coeff_next
      int run, level;
      lastTree->getValue(run, level);
      epr("dct_coeff_next : i %d -> %d, run %d, level %d\n", index,
          index + run + 1, run, level);
      index += run + 1;
      assert(index < 64);
      bh.dct_zz[index] = level;
    }
    fileReader.checkAndDiscard("10"); // end_of_block
  }
}

inline double ccc(int x) {
  if (x == 0)
    return 1.0 / std::sqrt(2.0);
  else
    return 1.0;
}
inline int rounding(double x) {
  if (x > 0.0)
    return int(floor(x + 0.5));
  else
    return int(ceil(x - 0.5));
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

#define SIGN(v) ((v < 0) ? -1 : ((v > 0) ? 1 : 0))
void MPEGDecoder::recoverBlock(int index) {
  int i;
  // common part of block recovering
  for (int m = 0; m < 8; m++) {
    for (int n = 0; n < 8; n++) {
      i = scan[m][n];
      if (mbh.isIntra)
        ob.recon[m][n] = (2 * bh.dct_zz[i] * sh.quantizer_scale *
                          sequenceHeader.intra_quantizer_matrix[m][n]) /
                         16;
      else
        ob.recon[m][n] =
            (((2 * bh.dct_zz[i]) + SIGN(bh.dct_zz[i])) * sh.quantizer_scale *
             sequenceHeader.nonintra_quantizer_matrix[m][n]) /
            16;
      if ((int(ob.recon[m][n]) & 1) == 0)
        ob.recon[m][n] -= SIGN(ob.recon[m][n]);
      if (ob.recon[m][n] > 2047)
        ob.recon[m][n] = 2047;
      if (ob.recon[m][n] < -2048)
        ob.recon[m][n] = -2048;
      if (bh.dct_zz[i] == 0 && !pheader.isI())
        ob.recon[m][n] = 0;
    }
  }

  if (mbh.isIntra) {
    double *dcpast;
    if (index < 4)
      dcpast = &ob.dct_dc_y_past;
    else if (index == 4)
      dcpast = &ob.dct_dc_cb_past;
    else
      dcpast = &ob.dct_dc_cr_past;

    ob.recon[0][0] = bh.dct_zz[0] * 8;
    if ((index == 0 || index > 3) &&
        (ob.macroblock_addr - ob.past_intra_addr > 1)) {
      // first y, cb, cr
      ob.recon[0][0] = 128 * 8 + ob.recon[0][0];
    } else {
      ob.recon[0][0] = *dcpast + ob.recon[0][0];
    }
    *dcpast = ob.recon[0][0];
  }
  ob.idct();
}
int count = 0;
void OriginalBlock::idct() {
  // init lookup table to speed up
  double idct_table[8][8] = {{0}};
  for (int i = 0; i < 8; i++)
    for (int j = 0; j < 8; j++)
      idct_table[i][j] =
          ccc(i) * std::cos(((2.0 * j + 1.0) * double(i) * M_PI) / 16.0) / 2.0;

  /*fprintf(stderr, "before idct %d\n", count);
  for(int u = 0; u < 8; u++){
      for(int v = 0; v < 8; v++){
          fprintf(stderr, "%lf ", recon[u][v]);
      }
      fprintf(stderr, "\n");
      }*/

  double out[8][8] = {{0}};
  for (int j = 0; j < 8; j++)
    for (int i = 0; i < 8; i++)
      for (int u = 0; u < 8; u++)
        for (int v = 0; v < 8; v++)
          out[j][i] += recon[v][u] * idct_table[u][i] * idct_table[v][j];
  for (int u = 0; u < 8; u++)
    for (int v = 0; v < 8; v++) {
      assert(out[u][v] >= -256 && out[u][v] <= 255);
      recon[u][v] = out[u][v];
    }

  /*fprintf(stderr, "after idct %d\n", count++);
  for(int u = 0; u < 8; u++){
      for(int v = 0; v < 8; v++){
          fprintf(stderr, "%lf ", recon[u][v]);
      }
      fprintf(stderr, "\n");
      }
  if(count == 10)
  exit(0);*/
}

void MPEGDecoder::writeBlock(int index, int addr) {
  // calculate row, column
  int mb_row = addr / ob.macroBlockWidth;
  int mb_col = addr % ob.macroBlockWidth;
  epr("writeblock: addr %d, width %d row %d col %d top %d row %d\n", addr,
      ob.macroBlockWidth, mb_row, mb_col, mb_row * 16, mb_col * 16);
  c_buf->serial = pheader.serial;
  if (index <= 3) { // Y
    int top = mb_row * 16;
    int left = mb_col * 16;
    switch (index) {
    case 0:
      break;
    case 1:
      left += 8;
      break;
    case 2:
      top += 8;
      break;
    case 3:
      left += 8;
      top += 8;
      break;
    default:
      assert(false);
    }
    for (int i = 0; i < 8; ++i)
      for (int j = 0; j < 8; ++j)
        c_buf->y[i + top][j + left] = fit(ob.recon[i][j]);
  } else {
    int top = mb_row * 8;
    int left = mb_col * 8;
    if (index == 4) {
      for (int i = 0; i < 8; ++i)
        for (int j = 0; j < 8; ++j)
          c_buf->cb[i + top][j + left] = fit(ob.recon[i][j]);
    } else {
      for (int i = 0; i < 8; ++i)
        for (int j = 0; j < 8; ++j)
          c_buf->cr[i + top][j + left] = fit(ob.recon[i][j]);
    }
  }
}

template <int W>
void MPEGDecoder::applyMotionCompensation(double pel[8][8],
                                          double pel_past[][W], int recon_right,
                                          int recon_down, int index, bool half,
                                          int macroblock_addr, int mb_width) {
  int right, down, right_half, down_half;
  if (index < 4) {
    right = recon_right >> 1;
    down = recon_down >> 1;
    right_half = recon_right - 2 * right;
    down_half = recon_down - 2 * down;
  } else {
    right = (recon_right / 2) >> 1;
    down = (recon_down / 2) >> 1;
    right_half = recon_right / 2 - 2 * right;
    down_half = recon_down / 2 - 2 * down;
  }

  double weight = half ? 0.5 : 1;

  int mb_row = macroblock_addr / mb_width;
  int mb_col = macroblock_addr % mb_width;
  int top = mb_row * 16;
  int left = mb_col * 16;
  switch (index) {
  case 0:
    break;
  case 1:
    left += 8;
    break;
  case 2:
    top += 8;
    break;
  case 3:
    left += 8, top += 8;
    break;
  default:
    top /= 2, left /= 2;
  }
  if (!right_half && !down_half) {
    for (int i = 0; i < 8; ++i)
      for (int j = 0; j < 8; ++j)
        pel[i][j] +=
            rounding(pel_past[top + i + down][left + j + right]) * weight;
  } else if (!right_half && down_half) {
    for (int i = 0; i < 8; ++i)
      for (int j = 0; j < 8; ++j)
        pel[i][j] += rounding((pel_past[top + i + down][left + j + right] +
                               pel_past[top + i + down + 1][left + j + right]) /
                              2) *
                     weight;
  } else if (right_half && !down_half) {
    for (int i = 0; i < 8; ++i)
      for (int j = 0; j < 8; ++j) {
        pel[i][j] += rounding((pel_past[top + i + down][left + j + right] +
                               pel_past[top + i + down][left + j + right + 1]) /
                              2) *
                     weight;
      }
  } else if (right_half && down_half) {
    for (int i = 0; i < 8; ++i)
      for (int j = 0; j < 8; ++j) {
        pel[i][j] +=
            rounding((pel_past[top + i + down][left + j + right] +
                      pel_past[top + i + down][left + j + right + 1] +
                      pel_past[top + i + down + 1][left + j + right] +
                      pel_past[top + i + down + 1][left + j + right + 1]) /
                     4) *
            weight;
      }
  }
}

void MPEGDecoder::addMotionVector(int index) {
  bool has_f = mbh.isMotionForward;
  bool has_b = mbh.isMotionBackward;
  bool half = has_f && has_b;
  if (has_f) {
    f_buf->serial = pheader.serial;
    if (index < 4)
      applyMotionCompensation(ob.recon, f_buf->y, ob.recon_right_for,
                              ob.recon_down_for, index, half,
                              ob.macroblock_addr, ob.macroBlockWidth);
    else if (index == 4)
      applyMotionCompensation(ob.recon, f_buf->cb, ob.recon_right_for,
                              ob.recon_down_for, index, half,
                              ob.macroblock_addr, ob.macroBlockWidth);
    else
      applyMotionCompensation(ob.recon, f_buf->cr, ob.recon_right_for,
                              ob.recon_down_for, index, half,
                              ob.macroblock_addr, ob.macroBlockWidth);
  }
  if (has_b) {
    b_buf->serial = pheader.serial;
    if (index < 4)
      applyMotionCompensation(ob.recon, b_buf->y, ob.recon_right_back,
                              ob.recon_down_back, index, half,
                              ob.macroblock_addr, ob.macroBlockWidth);
    else if (index == 4)
      applyMotionCompensation(ob.recon, b_buf->cb, ob.recon_right_back,
                              ob.recon_down_back, index, half,
                              ob.macroblock_addr, ob.macroBlockWidth);
    else
      applyMotionCompensation(ob.recon, b_buf->cr, ob.recon_right_back,
                              ob.recon_down_back, index, half,
                              ob.macroblock_addr, ob.macroBlockWidth);
  }
}
void MPEGDecoder::output(YCbCrBuffer *buf) {
  fprintf(stderr, "output serial %d\n", buf->serial);
  uint &h = sequenceHeader.h;
  uint &v = sequenceHeader.v;
  byte buffer[3 * 576 * 768];
  for (int i = 0; i < 320; ++i)
    for (int j = 0; j < 240; ++j) {
      buffer[0 * v * h + j * h + i] =
          std::max(0., std::min(255., 255. / 219 * (buf->y[j][i] - 16) +
                                          255. / 112 * 0.701 *
                                              (buf->cr[j / 2][i / 2] - 128)));
      buffer[1 * v * h + j * h + i] =
          std::max(0., std::min(255., 255. / 219 * (buf->y[j][i] - 16) -
                                          255. / 112 * 0.886 * 0.114 / 0.587 *
                                              (buf->cb[j / 2][i / 2] - 128) -
                                          255. / 112 * 0.701 * 0.299 / 0.587 *
                                              (buf->cr[j / 2][i / 2] - 128)));
      buffer[2 * v * h + j * h + i] =
          std::max(0., std::min(255., 255. / 219 * (buf->y[j][i] - 16) +
                                          255. / 112 * 0.886 *
                                              (buf->cb[j / 2][i / 2] - 128)));
    }

  static int last_tick = 0;
  double freq = 1. / 40;
  while (clock() - last_tick < freq * CLOCKS_PER_SEC)
    ;
  last_tick = clock();

  main_disp.display(CImg<byte>(buffer, h, v, 1, 3, true));

  // pause
  if (main_disp.is_keySPACE()) {
    while (main_disp.is_keySPACE())
      ;
    while (!main_disp.is_keySPACE())
      ;
    while (main_disp.is_keySPACE())
      ;
  }
}

void MPEGDecoder::nextStartCode() {
  while (!fileReader.byteAligned())
    assert(!fileReader.nextBit()); // zerobit
  while (!fileReader.checkBits(start_code)) {
    byte b;
    nextByte(b);
    assert(b == 0); // zerobyte
  }
  // epr("after nextstartcode\n");
  fileReader.print();
}
