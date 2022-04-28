#include <cstdio>
#include <cmath>
#ifndef UTIL_H
#define UTIL_H
const double pi = acos(-1);
const double sq8 = sqrt(8);
double cn[5];
double sn[5];
int Clip(int& x,int m,int M){
	if(x>M)x=M;
	if(x<m)x=m;
	return x;
}
inline int div_round(int a,int b){return (int)round((double)a/b);}
inline static void ToRGB(int& y, int& cb, int& cr) {
	cb -= 128; cr -= 128;
  int r = round(y + 1.402 * cr);
  int g = round(y - 0.34414 * cb - 0.71414 * cr);
  int b = round(y + 1.772 * cb);
  y = Clip(r, 0, 255); cb = Clip(g, 0, 255); cr = Clip(b, 0, 255);
}


int Sign(int a){
	return (a>0) - (a<0);
}
void init_idct(){
	for(int i=1;i<=4;i++){
		cn[i] = cos(i*pi/16.0);
		sn[i] = sin(i*pi/16.0);
	}
}
void idct_1D(double n[8][8], double o[8][8], int r){
	double t[8];
	t[0] =  o[r][0]/sq8  + o[r][4]/sq8;
	t[1] = -o[r][4]/sq8  + o[r][0]/sq8;
	t[2] =  o[r][2]/2.0 *sn[2] - o[r][6]/2.0*cn[2];
	t[3] =  o[r][6]/2.0 *sn[2] + o[r][2]/2.0*cn[2];
	t[4] =  o[r][1]/2.0 *cn[1] + o[r][7]/2.0*sn[1];
	t[5] = -o[r][7]/2.0 *cn[1] + o[r][1]/2.0*sn[1];
	t[6] =  o[r][3]/2.0 *cn[3] + o[r][5]/2.0*sn[3];
	t[7] = -o[r][5]/2.0 *cn[3] + o[r][3]/2.0*sn[3];

	n[0][r] =   t[0] +t[3];
	n[1][r] =   t[1] +t[2];
	n[2][r] =  -t[2] +t[1];
	n[3][r] =  -t[3] +t[0];
	n[4][r] =   t[4] +t[6];
	n[5][r] =  (t[5] +t[7]) *cn[4];
	n[6][r] =  (-t[6] +t[4])*cn[4];
	n[7][r] =   -t[7] +t[5];

	t[0] = n[0][r];
	t[1] = n[1][r];
	t[2] = n[2][r];
	t[3] = n[3][r];
	t[4] = n[4][r];
	t[5] = n[5][r] +n[6][r];
	t[6] =-n[6][r] +n[5][r];
	t[7] = n[7][r];
	/*
		 0 1 2 3 4 5 6 7
		 0 1 6 3 7 2 5 4
	 * */	
	n[0][r] =  t[0] +t[4];
	n[1][r] =  t[1] +t[5];
	n[5][r] =  t[2] +t[6];
	n[3][r] =  t[3] +t[7];
	n[7][r] = -t[4] +t[0];
	n[6][r] = -t[5] +t[1];
	n[2][r] = -t[6] +t[2];
	n[4][r] = -t[7] +t[3];
}
void idct(int block[][8]){
	double d_block[2][8][8];
	for(int i=0;i<8;i++)
		for(int j=0;j<8;j++) d_block[0][i][j] = block[i][j];
	for(int i=0;i<8;i++){
		idct_1D(d_block[1],d_block[0],i);
	}
	for(int i=0;i<8;i++)
		idct_1D(d_block[0],d_block[1],i);
	for(int i=0;i<8;i++)
		for(int j=0;j<8;j++) block[i][j] = d_block[0][i][j] ;
}
// fast IDCT code from http://read.pudn.com/downloads33/sourcecode/multimedia/streaming/106711/dct/idct.c__.htm

#define W1 2841                 /* 2048*sqrt(2)*cos(1*pi/16) */
#define W2 2676                 /* 2048*sqrt(2)*cos(2*pi/16) */
#define W3 2408                 /* 2048*sqrt(2)*cos(3*pi/16) */
#define W5 1609                 /* 2048*sqrt(2)*cos(5*pi/16) */
#define W6 1108                 /* 2048*sqrt(2)*cos(6*pi/16) */
#define W7 565                  /* 2048*sqrt(2)*cos(7*pi/16) */

/* two dimensional inverse discrete cosine transform */
void IDCT(int *const block) {
  static int *blk;
  static long i;
  static long X0, X1, X2, X3, X4, X5, X6, X7, X8;

  for (i = 0; i < 8; i++) {
    blk = block + (i << 3);
    if (!((X1 = blk[4] << 11) | (X2 = blk[6]) | (X3 = blk[2]) |
          (X4 = blk[1]) | (X5 = blk[7]) | (X6 = blk[5]) | (X7 = blk[3]))) {
      blk[0] = blk[1] = blk[2] = blk[3] = blk[4] = blk[5] = blk[6] =
        blk[7] = blk[0] << 3;
      continue;
    }

    X0 = (blk[0] << 11) + 128;    /* for proper rounding in the fourth stage  */

    /* first stage  */
    X8 = W7 * (X4 + X5);
    X4 = X8 + (W1 - W7) * X4;
    X5 = X8 - (W1 + W7) * X5;
    X8 = W3 * (X6 + X7);
    X6 = X8 - (W3 - W5) * X6;
    X7 = X8 - (W3 + W5) * X7;

    /* second stage  */
    X8 = X0 + X1;
    X0 -= X1;
    X1 = W6 * (X3 + X2);
    X2 = X1 - (W2 + W6) * X2;
    X3 = X1 + (W2 - W6) * X3;
    X1 = X4 + X6;
    X4 -= X6;
    X6 = X5 + X7;
    X5 -= X7;

    /* third stage  */
    X7 = X8 + X3;
    X8 -= X3;
    X3 = X0 + X2;
    X0 -= X2;
    X2 = (181 * (X4 + X5) + 128) >> 8;
    X4 = (181 * (X4 - X5) + 128) >> 8;

    /* fourth stage  */
    blk[0] = (X7 + X1) >> 8;
    blk[1] = (X3 + X2) >> 8;
    blk[2] = (X0 + X4) >> 8;
    blk[3] = (X8 + X6) >> 8;
    blk[4] = (X8 - X6) >> 8;
    blk[5] = (X0 - X4) >> 8;
    blk[6] = (X3 - X2) >> 8;
    blk[7] = (X7 - X1) >> 8;
  }

  for (i = 0; i < 8; i++) {
    blk = block + i;
    if (!((X1 = (blk[8 * 4] << 8)) | (X2 = blk[8 * 6]) | (X3 = blk[8 * 2]) |
          (X4 = blk[8 * 1]) | (X5 = blk[8 * 7]) | (X6 = blk[8 * 5]) | (X7 = blk[8 * 3]))) {
      blk[8 * 0] = blk[8 * 1] = blk[8 * 2] = blk[8 * 3] = blk[8 * 4] =
        blk[8 * 5] = blk[8 * 6] = blk[8 * 7] = (blk[8 * 0] + 32) >> 6;
      continue;
    }

    X0 = (blk[8 * 0] << 8) + 8192;

    /* first stage  */
    X8 = W7 * (X4 + X5) + 4;
    X4 = (X8 + (W1 - W7) * X4) >> 3;
    X5 = (X8 - (W1 + W7) * X5) >> 3;
    X8 = W3 * (X6 + X7) + 4;
    X6 = (X8 - (W3 - W5) * X6) >> 3;
    X7 = (X8 - (W3 + W5) * X7) >> 3;

    /* second stage  */
    X8 = X0 + X1;
    X0 -= X1;
    X1 = W6 * (X3 + X2) + 4;
    X2 = (X1 - (W2 + W6) * X2) >> 3;
    X3 = (X1 + (W2 - W6) * X3) >> 3;
    X1 = X4 + X6;
    X4 -= X6;
    X6 = X5 + X7;
    X5 -= X7;

    /* third stage  */
    X7 = X8 + X3;
    X8 -= X3;
    X3 = X0 + X2;
    X0 -= X2;
    X2 = (181 * (X4 + X5) + 128) >> 8;
    X4 = (181 * (X4 - X5) + 128) >> 8;

    /* fourth stage  */
    blk[8 * 0] = (X7 + X1) >> 14;
    blk[8 * 1] = (X3 + X2) >> 14;
    blk[8 * 2] = (X0 + X4) >> 14;
    blk[8 * 3] = (X8 + X6) >> 14;
    blk[8 * 4] = (X8 - X6) >> 14;
    blk[8 * 5] = (X0 - X4) >> 14;
    blk[8 * 6] = (X3 - X2) >> 14;
    blk[8 * 7] = (X7 - X1) >> 14;
  }
}
#endif
