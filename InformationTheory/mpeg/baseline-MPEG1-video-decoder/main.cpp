#include <cstdio>
#include "magic_code.h"
#include "bit_reader.h"
#include "video.h"

int main(int argc, char *argv[]) {
    FILE *file = fopen(argv[1], "rb");
    BitReader stream(file);
    VideoDecoder decoder;
    decoder.video_sequence(stream);
    fclose(file);
    return 0;
}
