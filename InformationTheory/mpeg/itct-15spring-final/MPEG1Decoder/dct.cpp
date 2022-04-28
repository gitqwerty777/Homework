#define xadd3(xa,xb,xc,xd,h) p=xa+xb, n=xa-xb, xa=p+xc+h, xb=n+xd+h, xc=p-xc+h, xd=n-xd+h // triple-butterfly-add (and possible rounding)
#define xmul(xa,xb,k1,k2,sh) n=k1*(xa+xb), p=xa, xa=(n+(k2-k1)*xb)>>sh, xb=(n-(k2+k1)*p)>>sh // butterfly-mul equ.(2)

static void idct1(int *x, int *y, int ps, int half) // 1D-IDCT
{
    int p, n;
    x[0] <<= 9, x[1] <<= 7, x[3] *= 181, x[4] <<= 9, x[5] *= 181, x[7] <<= 7;
    xmul(x[6], x[2], 277, 669, 0);
    xadd3(x[0], x[4], x[6], x[2], half);
    xadd3(x[1], x[7], x[3], x[5], 0);
    xmul(x[5], x[3], 251, 50, 6);
    xmul(x[1], x[7], 213, 142, 6);
    y[0 * 8] = (x[0] + x[1]) >> ps;
    y[1 * 8] = (x[4] + x[5]) >> ps;
    y[2 * 8] = (x[2] + x[3]) >> ps;
    y[3 * 8] = (x[6] + x[7]) >> ps;
    y[4 * 8] = (x[6] - x[7]) >> ps;
    y[5 * 8] = (x[2] - x[3]) >> ps;
    y[6 * 8] = (x[4] - x[5]) >> ps;
    y[7 * 8] = (x[0] - x[1]) >> ps;
}

void idct(int *b) // 2D 8x8 IDCT
{
    int i, b2[64];
    for (i = 0; i<8; i++) idct1(b + i * 8, b2 + i, 9, 1 << 8); // row
    for (i = 0; i<8; i++) idct1(b2 + i * 8, b + i, 12, 1 << 11); // col
}