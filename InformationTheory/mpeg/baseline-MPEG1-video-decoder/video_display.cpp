#include <ctime>
#include "CImg.h"
#include "video.h"
using namespace cimg_library;

void VideoDecoder::display(YCbCrBuffer *buf) {
    byte buffer[3*576*768];
    for(int i=0; i<320; ++i)
        for(int j=0; j<240; ++j) {
            buffer[0*v_size*h_size + j*h_size + i] = std::max(0., std::min(255., 255./219*(buf->y[j][i]-16) + 255./112*0.701*(buf->cr[j/2][i/2]-128)));
            buffer[1*v_size*h_size + j*h_size + i] = std::max(0., std::min(255., 255./219*(buf->y[j][i]-16) - 255./112*0.886*0.114/0.587*(buf->cb[j/2][i/2]-128) - 255./112*0.701*0.299/0.587*(buf->cr[j/2][i/2]-128)));
            buffer[2*v_size*h_size + j*h_size + i] = std::max(0., std::min(255., 255./219*(buf->y[j][i]-16) + 255./112*0.886*(buf->cb[j/2][i/2]-128)));
        }

    // wait fps
    static int last_tick = 0;
    double freq = 1./40;
    while(clock()-last_tick < freq*CLOCKS_PER_SEC);
    last_tick = clock();

    main_disp.display(CImg<byte>(buffer, h_size, v_size, 1, 3, true));

    // pause
    if(main_disp.is_keySPACE()) {
        while(main_disp.is_keySPACE());
        while(!main_disp.is_keySPACE());
        while(main_disp.is_keySPACE());
    }
}
