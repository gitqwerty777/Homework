#include "MPEG1Decoder.h"
#include <GL/glut.h>

MPEG1Data mpg;
Pixel *currentFrame;
bool pause_ = false;
std::thread *t_decode = NULL;
std::thread *t_cli = NULL;

void decodeThread()
{
    bool finished = false;
    while (true) {
        while (mpg.seeking) usleep(1);
        if (mpg.was_seeking) finished = false;
        if (mpg.terminate) break;
        if (finished) {
            usleep(1000);
            continue;
        }
        if (mpg.next_start_code == 0x00000100) mpg.next_start_code = 0x000001b8;
        switch (mpg.next_start_code) {
        case 0x000001b3:
            decodeHeader(mpg);
            break;
        case 0x000001b8:
            mpg.decoding = true;
            try {
                decodeGOP(mpg);
            }
            catch (const char *msg) {
                fprintf(stderr, "%s\n", msg);
                finished = true;
                if (mpg.forward_ref != NULL) {
                    delete[] mpg.forward_ref;
                    mpg.forward_ref = NULL;
                }
                if (mpg.backward_ref != NULL) {
                    mpg.mtx_frames.lock();
                    mpg.frames.push(mpg.backward_ref);
                    mpg.mtx_frames.unlock();
                    mpg.backward_ref = NULL;
                }
            }
            mpg.decoding = false;
            break;
        case 0x000001b7:
            printf("decode finished\n");
            finished = true;
            if (mpg.forward_ref != NULL) {
                delete[] mpg.forward_ref;
                mpg.forward_ref = NULL;
            }
            if (mpg.backward_ref != NULL) {
                mpg.mtx_frames.lock();
                mpg.frames.push(mpg.backward_ref);
                mpg.mtx_frames.unlock();
                mpg.backward_ref = NULL;
            }
            break;
        default:
            fprintf(stderr, "unknown start code %08x\n", mpg.next_start_code);
            exit(1);
        }
    }
}

void fetchFrame(int value)
{
    mpg.mtx_frames.lock();
    if (!mpg.frames.empty()) {
        currentFrame = mpg.frames.front();
        mpg.frames.pop();
        glutPostRedisplay();
    }
    mpg.mtx_frames.unlock();
    if (pause_) return;
    glutTimerFunc(1000 / mpg.fps, fetchFrame, 0);
}

void display()
{
    if (currentFrame != NULL) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawPixels(mpg.width, mpg.height, GL_RGB, GL_UNSIGNED_BYTE, currentFrame);
        glutSwapBuffers();
        delete [] currentFrame;
        currentFrame = NULL;
    }
}

void mouseClick(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        if (pause_) {
            pause_ = false;
            fetchFrame(0);
        }
        else {
            pause_ = true;
        }
    }
}

bool seek(int frame_id)
{
    if (frame_id < 0 || frame_id >= mpg.index.size()) return false;
    mpg.terminate = true;
    mpg.seeking = true;
    while (mpg.decoding) usleep(1);
    mpg.terminate = false;
    int pos = mpg.index[frame_id];
    printf("seek to frame #%d, pos = %08x\n", frame_id, pos);
    mpg.mtx_frames.lock();
    while (!mpg.frames.empty()) {
        delete[] mpg.frames.front();
        mpg.frames.pop();
    }
    mpg.mtx_frames.unlock();
    if (mpg.forward_ref == NULL) mpg.forward_ref = new Pixel[mpg.width * mpg.height];
    std::fill_n(mpg.forward_ref, mpg.width * mpg.height, Pixel(0, 0, 0));
    if (mpg.backward_ref == NULL) mpg.backward_ref = new Pixel[mpg.width * mpg.height];
    std::fill_n(mpg.backward_ref, mpg.width * mpg.height, Pixel(0, 0, 0));
    mpg.stream.setpos(pos);
    mpg.next_start_code = 0x00000100;
    mpg.was_seeking = true;
    mpg.seeking = false;
    return true;
}

void printCommands()
{
    std::cout
        << "Commands:\n"
        << "g <frame_id>: seek to frame_id\n"
        << "v: verbose mode (press enter to quit)\n"
        << "q: quit program\n"
        ;
    std::cout.flush();
}

void cliThread()
{
    std::string buf, cmd;
    std::getline(std::cin, buf);
    mpg.verbose = false;
    printCommands();
    while (true) {
        std::cout << ">>> ";
        std::getline(std::cin, buf);
        if (std::cin.eof()) break;
        trim(buf);
        if (buf == "") continue;
        std::stringstream ss(buf);
        ss >> cmd;
        if (cmd == "v") {
            mpg.verbose = true;
            std::getline(std::cin, cmd);
            mpg.verbose = false;
        }
        else if (cmd == "q") {
            break;
        }
        else if (cmd == "g") {
            int frame_id = -1;
            ss >> frame_id;
            if (!seek(frame_id)) {
                std::cout << "Invalid frame_id" << std::endl;
            }
        }
        else {
            std::cout << "Unknown command " << cmd << std::endl;
            printCommands();
        }
    }
    mpg.terminate = true;
    t_decode->join();
    exit(0);
}

int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    try {
        mpg.stream.open(argv[1]);
    }
    catch (const char *msg){
        fprintf(stderr, "Error: %s\n", msg);
        return 1;
    }
    if (mpg.stream.nextbits(32) != 0x000001b3) {
        fprintf(stderr, "Error: not an MPEG1 sequence\n");
        return 1;
    }
    decodeHeader(mpg);
    Tables::initTables(mpg.stream);

    mpg.verbose = false;
    decodeGOPIndex(mpg);
    try {
        while (mpg.next_start_code != 0x000001b7) {
            if (mpg.next_start_code == 0x000001b3) decodeHeader(mpg);
            else decodeGOPIndex(mpg);
        }
    }
    catch (const char *msg) {
        fprintf(stderr, "%s\n", msg);
    }
    mpg.stream.setpos(0);
    mpg.next_start_code = mpg.stream.nextbits(32);
    mpg.verbose = true;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(mpg.width_orig, mpg.height_orig);
    glutCreateWindow("MPEG1Decoder");

    glutDisplayFunc(display);
    glutTimerFunc(1000 / mpg.fps, fetchFrame, 0);
    glutMouseFunc(mouseClick);

    t_decode = new std::thread(decodeThread);
    t_cli = new std::thread(cliThread);
    glutMainLoop();

    return 0;
}
