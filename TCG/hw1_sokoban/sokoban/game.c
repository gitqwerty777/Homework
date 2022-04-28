// by Yunghsien Chung (hiiragi4000)

#include<ctype.h>
#include<stdbool.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"getch.h"
#include"sokoban.h"

// current board
Board Brd;
// history of moves
Vector_Move History = {.data=0, .size=0, .capacity=0};

// draw the board
// if the flag 'clear' is set, erase the previous board
static void Draw(bool const clear){
    if(clear){
        for(int i=1; i<=Brd.height+3; ++i){
            printf("\033[1F\033[2K");
        }
    }
    for(int i=0; i<=Brd.height-1; ++i) for(int j=0; j<=Brd.width-1; ++j){
        switch(Brd.board[i][j]){
            case '#':
                printf("\033[1;31;41m#\033[m");
                break;
            case '@':
                printf("\033[1;35m@\033[m");
                break;
            case '+':
                printf("\033[1;35;42m+\033[m");
                break;
            case '$':
                printf("\033[1;33m$\033[m");
                break;
            case '*':
                printf("\033[1;33;42m*\033[m");
                break;
            case '.':
                printf("\033[1;32;42m.\033[m");
                break;
            case '-':
                putchar('-');
                break;
        }
        if(j == Brd.width-1){
            putchar('\n');
        }
    }
    printf("\nMove Count: \033[1m%3d\033[m\n\n", History.size);
    fflush(stdout);
}

int main(int argc, char **argv){

// parse the arguments
#define SOKOBAN_WRONG_ARG do{\
    puts("Usage: sokoban [-i filename] [-o filename] [-s stage]");\
    puts("-i filename: read the puzzles from file \'filename\'.");\
    puts("-o filename: if specified, write the solutions to file \'filename\'.");\
    puts("-s stage: if specified, start from Stage \'stage\'.");\
    return 1;\
}while(0)
    char *input_filename = 0, *output_filename = 0;
    int start_stage = 0;
    for(int i=1; i<=argc-1; ++i){
        if(!strcmp(argv[i], "-i")){
            if(++i==argc || input_filename){
                SOKOBAN_WRONG_ARG;
            }
            input_filename = argv[i];
        }else if(!strcmp(argv[i], "-o")){
            if(++i==argc || output_filename){
                SOKOBAN_WRONG_ARG;
            }
            output_filename = argv[i];
        }else if(!strcmp(argv[i], "-s")){
            if(++i==argc || start_stage){
                SOKOBAN_WRONG_ARG;
            }
            start_stage = atoi(argv[i]);
            if(start_stage <= 0){
                SOKOBAN_WRONG_ARG;
            }
        }else{
            SOKOBAN_WRONG_ARG;
        }
    }
    if(!input_filename){
        SOKOBAN_WRONG_ARG;
    }
    if(!start_stage){
        start_stage = 1;
    }
#undef SOKOBAN_WRONG_ARG

// verify the existence of 'input_filename' and try to open 'output_filename'
    FILE *fin = fopen(input_filename, "r");
    if(!fin){
        printf("Error in opening the file \'%s\'\n", input_filename);
        return 1;
    }
    FILE *fout = fopen(output_filename, "w");
    if(output_filename && !fout){
        printf("Error in opening the file \'%s\'\n", output_filename);
        return 1;
    }

// start the game
#define SOKOBAN_OUTPUT_SOL do{\
    fprintf(fout, "%d\n", History.size);\
    for(int i=0; i<History.size; ++i){\
        fputc(History.data[i].push? "UDLR"[History.data[i].dir]: "udlr"[History.data[i].dir], fout);\
    }\
    fputc('\n', fout);\
}while(0)
#define SOKOBAN_END_GAME do{\
    puts("\033[1;32mThanks for Playing!\033[m");\
    fclose(fin);\
    if(fout){\
        fclose(fout);\
    }\
    return 0;\
}while(0)
    puts("\033[1;36mLegend:\n");
    puts("\033[1;31;41m#\033[m\tWall");
    puts("\033[1;35m@\033[m\tPlayer");
    puts("\033[1;35;42m+\033[m\tPlayer on Goal Square");
    puts("\033[1;33m$\033[m\tBox");
    puts("\033[1;33;42m*\033[m\tBox on Goal Square");
    puts("\033[1;32;42m.\033[m\tGoal Square");
    puts("-\tFloor");
    putchar('\n');
    puts("Use \033[1;35mW\033[m, \033[1;35mA\033[m, \033[1;35mS\033[m, \033[1;35mD\033[m, or \033[1;35marrow keys\033[m to move.");
    puts("Press \033[1;35mP\033[m to pause.");
    puts("Press \033[1;35mU\033[m to undo.");
    putchar('\n');
    for(int stage=1; Get_Board(fin, &Brd); ++stage){
        if(stage < start_stage){
            if(fout){
                fprintf(fout, "0\n\n");
            }
            continue;
        }
restart:
        printf("\033[1;36mStage #%d:\033[m\n\n", stage);
        Draw(false);
        while(Brd.m_cnt){
            char cmd = tolower(getch());
            int dir = -1; // -1: error, 0-3: move, 4: do nothing
            switch(cmd){
                case 27: // ESC character, handling arrow keys
                    getch(); // '[' character
                    cmd = getch();
                    switch(cmd){
                        case 'A':
                            dir=UP; break;
                        case 'B':
                            dir=DOWN; break;
                        case 'C':
                            dir=RIGHT; break;
                        case 'D':
                            dir=LEFT; break;
                    }
                    break;
                case 'w':
                    dir=UP; break;
                case 'a':
                    dir=LEFT; break;
                case 's':
                    dir=DOWN; break;
                case 'd':
                    dir=RIGHT; break;
                case 'p':
                    puts("re(\033[1;35mS\033[m)ume");
                    puts("res(\033[1;35mT\033[m)art");
                    puts("(\033[1;35mN\033[m)ext Stage");
                    puts("(\033[1;35mQ\033[m)uit the Game\n");
                    fflush(stdout);
                    cmd = tolower(getch());
                    while(!strchr("stnq", cmd)){
                        putchar('\a');
                        cmd = tolower(getch());
                    }
                    switch(cmd){
                        case 's':
                            for(int i=1; i<=5; ++i){
                                printf("\033[1F\033[2K");
                            }
                            fflush(stdout);
                            continue;
                        case 't':
                            for(int i=1; i<=Brd.height+10; ++i){
                                printf("\033[1F\033[2K");
                            }
                            fflush(stdout);
                            for(int i=History.size-1; i>=0; --i){
                                Undo(&Brd, History.data[i]);
                            }
                            VM_clear(&History);
                            goto restart;
                        case 'n':
                            if(fout){
                                fprintf(fout, "0\n\n");
                            }
                            goto new_stage;
                        case 'q':
                            VM_clear(&History);
                            SOKOBAN_END_GAME;
                    }
                    break;
                case 'u':
                    if(History.size){
                        Undo(&Brd, History.data[History.size-1]);
                        VM_pop_back(&History);
                        dir = 4;
                    }
                    break;
            }
            if(dir==-1 || (dir!=4 && !Do_move(&Brd, dir, &History))){
                putchar('\a');
            }else Draw(true);
        }
        puts("\033[1;33mStage Clear!\033[m\n");
        if(fout){
            SOKOBAN_OUTPUT_SOL;
        }
new_stage:
        VM_clear(&History);
    }
    SOKOBAN_END_GAME;
#undef SOKOBAN_OUTPUT_SOL
#undef SOKOBAN_END_GAME

}
