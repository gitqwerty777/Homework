#include <cstdio>
#include <stdlib.h>
#include "jpg_decode.h"

int writeBmp(const char *fname_t, unsigned char *image, int width, int height) {
    FILE          *fp_t = NULL;    // target file handler 
    int ceilWidth = (width*24+31)/32*4;
    byte *image_t = (byte*)malloc(height*ceilWidth);
    for(int i=0; i<height; ++i)
        for(int j=0; j<width; ++j)
            for(int k=0; k<3; ++k)
                image_t[i*ceilWidth + j*3 + k] = image[(height-i-1)*width*3 + j*3 + k];

    unsigned char header[54] = {
        0x42,        // identity : B
        0x4d,        // identity : M
        0, 0, 0, 0,  // file size
        0, 0,        // reserved1
        0, 0,        // reserved2
        54, 0, 0, 0, // RGB data offset
        40, 0, 0, 0, // struct BITMAPINFOHEADER size
        0, 0, 0, 0,  // bmp width
        0, 0, 0, 0,  // bmp height
        1, 0,        // planes
        24, 0,       // bit per pixel
        0, 0, 0, 0,  // compression
        0, 0, 0, 0,  // data size
        0, 0, 0, 0,  // h resolution
        0, 0, 0, 0,  // v resolution 
        0, 0, 0, 0,  // used colors
        0, 0, 0, 0   // important colors
    };

    // write to new bmp
    fp_t = fopen(fname_t, "wb");
    if (fp_t == NULL) {
        printf("fopen fname_t error\n");
        return -1;
    }

    // file size  
    int file_size = ceilWidth * height * 3 + 54;
    header[2] = (unsigned char)(file_size & 0x000000ff);
    header[3] = (file_size >> 8)  & 0x000000ff;
    header[4] = (file_size >> 16) & 0x000000ff;
    header[5] = (file_size >> 24) & 0x000000ff;

    // width
    header[18] = width & 0x000000ff;
    header[19] = (width >> 8)  & 0x000000ff;
    header[20] = (width >> 16) & 0x000000ff;
    header[21] = (width >> 24) & 0x000000ff;

    // height
    header[22] = height &0x000000ff;
    header[23] = (height >> 8)  & 0x000000ff;
    header[24] = (height >> 16) & 0x000000ff;
    header[25] = (height >> 24) & 0x000000ff;

    // write header
    fwrite(header, sizeof(unsigned char), 54, fp_t);
    // write image
    fwrite(image_t, sizeof(unsigned char), (size_t)(long)ceilWidth * height, fp_t);

    fclose(fp_t);

    return 0;
}

int main(int argc, char *argv[]) {
    if(argc != 2) {
        printf("usage: %s JPG_FILE_NAME\n", argv[0]);
        return 0;
    }
    FILE *file = fopen(argv[1], "r");
    JPGDecoder decoder(file);
    decoder.decode();
    int height = decoder.getHeight();
    int width = decoder.getWidth();
    byte *bitmap = decoder.getBitmap();
    writeBmp("out.bmp", bitmap, width, height);
    puts("bmp file wrote to 'out.bmp'");
    return 0;
}
