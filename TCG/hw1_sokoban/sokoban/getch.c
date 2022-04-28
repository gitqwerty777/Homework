// This file is adapted from https://stackoverflow.com/questions/7469139
// You don't need to know how it works

#include<stdio.h>
#include<termios.h>

static struct termios old, new;

// Initialize new terminal i/o settings
static void initTermios(int echo){
    tcgetattr(0, &old); // grab old terminal i/o settings
    new = old; // make new settings same as old settings
    new.c_lflag &= ~ICANON; // disable buffered i/o
    if(echo){
        new.c_lflag |= ECHO; // set echo mode
    }else{
        new.c_lflag &= ~ECHO; // set no echo mode
    }
    tcsetattr(0, TCSANOW, &new); // use these new terminal i/o settings now
}

// Restore old terminal i/o settings
static void resetTermios(){
    tcsetattr(0, TCSANOW, &old);
}

// Read 1 character - echo defines echo mode
static char getch_(int echo){
    char ch;
    initTermios(echo);
    ch = getchar();
    resetTermios();
    return ch;
}

// Read 1 character without echo
char getch(){
    return getch_(0);
}

// Read 1 character with echo
char getche(){
    return getch_(1);
}
