// by Yunghsien Chung (hiiragi4000)

// You don't need to know much about how it works.

#include<assert.h>
#include<ctype.h>
#include<stdbool.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"sokoban.h"

// If the format of the input file (puzzle) is invalid, the program terminates.
void Input_verifier(FILE *fin){
#define INVALID_INPUT do{\
    puts("The input file is invalid.");\
    exit(1);\
}while(0)
    char s[HEIGHT_MAX][WIDTH_MAX+2];
    while(fgets(s[0], sizeof(s[0]), fin)){
        int len = strlen(s[0]);
        if(len>5 || s[0][len-1]!='\n'){
            INVALID_INPUT;
        }
        s[0][--len] = 0;
        char *sp = strchr(s[0], ' ');
        if(!sp || strchr(sp+1, ' ')) INVALID_INPUT;
        int n = 0;
        for(char *it=s[0]; it!=sp; ++it){
            if(!isdigit(*it) || (it==s[0]&&*it=='0')) INVALID_INPUT;
            n = 10*n+(*it)-48;
        }
        int m = 0;
        for(char *it=sp+1; *it; ++it){
            if(!isdigit(*it) || (it==sp+1&&*it=='0')) INVALID_INPUT;
            m = 10*m+(*it)-48;
        }
        if(n > HEIGHT_MAX || m > WIDTH_MAX || n*m > AREA_MAX) INVALID_INPUT;
        for(int i=0; i<=n-1; ++i){
            if(!fgets(s[i], sizeof(s[i]), fin)) INVALID_INPUT;
            if((int)strlen(s[i])!=m+1 || s[i][m]!='\n') INVALID_INPUT;
            for(int j=0; j<=m-1; ++j){
                if(!strchr("#@+$*.-", s[i][j])) INVALID_INPUT;
            }
        }
        int bcnt=0, gcnt=0, pcnt=0;
        for(int i=0; i<=n-1; ++i) for(int j=0; j<=m-1; ++j){
            bcnt += (s[i][j] == '$');
            gcnt += (s[i][j] == '.' || s[i][j] == '+');
            pcnt += (s[i][j] == '@' || s[i][j] == '+');
        }
        if(!bcnt || bcnt!=gcnt || pcnt!=1) INVALID_INPUT;
    }
}

// If the format of the output file (solution) is invalid, the program terminates.
void Output_verifier(FILE *fin){
#define INVALID_OUTPUT do{\
    puts("The output file is invalid.");\
    exit(1);\
}while(0)
    char line1[20];
    for(int stage=1; fgets(line1, 20, fin); ++stage){
        int len = strlen(line1);
        if(len<2 || len>9 || line1[len-1]!='\n') INVALID_OUTPUT;
        line1[--len] = 0;
        int n = 0;
        for(int i=0; i<=len-1; ++i){
            if(!isdigit(line1[i]) || (len>1&&i==0&&line1[i]=='0')) INVALID_OUTPUT;
            n = 10*n+line1[i]-48;
        }
        char *buf = malloc(n+2);
        if(!fgets(buf, n+2, fin)) INVALID_OUTPUT;
        if((int)strlen(buf)!=n+1 || buf[n]!='\n') INVALID_OUTPUT;
        for(int i=0; i<=n-1; ++i){
            if(!strchr("udlr", tolower(buf[i]))) INVALID_OUTPUT;
        }
        free(buf);
    }
}

int main(int argc, char **argv){
    char *input=0, *output=0;
#define WRONG_ARG do{\
    puts("Usage: verifier [-i infile] [-o outfile]");\
    puts("-i infile: if specified, check if the file \'infile\' is a valid input");\
    puts("-o outfile: if specified, check if the file \'outfile\' is a valid output");\
    puts("if both -i and -o are specified, also check if \'outfile\' solves the puzzle \'infile\'");\
    exit(1);\
}while(0)
    for(int i=1; i<argc; ++i){
        if(!strcmp(argv[i], "-i")){
            if(++i==argc || input) WRONG_ARG;
            input = argv[i];
        }else if(!strcmp(argv[i], "-o")){
            if(++i==argc || output) WRONG_ARG;
            output = argv[i];
        }else WRONG_ARG;
    }
    if(!input && !output) WRONG_ARG;
    FILE *fpi=0, *fpo=0;
    if(input){
        fpi = fopen(input, "r");
        if(!fpi){
            printf("Error in opening the file \'%s\'\n", input);
            return 1;
        }
        Input_verifier(fpi);
    }
    if(output){
        fpo = fopen(output, "r");
        if(!fpo){
            printf("Error in opening the file \'%s\'\n", output);
            return 1;
        }
        Output_verifier(fpo);
    }
    if(input && output){
        rewind(fpi);
        rewind(fpo);
        Board puzzle;
        for(int stage=1; Get_Board(fpi, &puzzle); ++stage){
            printf("Stage #%d: ", stage);
            int n;
            if(fscanf(fpo, "%d ", &n)!=1 || n==0){
                puts("Wrong Answer");
                continue;
            }
            char *sol = malloc(n+2);
            if(!fgets(sol, n+2, fpo)){
                // never happens
                assert(0);
            }
            bool ac = true;
            for(int i=0; i<=n-1; ++i){
                static char *dir = "udlr";
                if(!Do_move(&puzzle, strchr(dir, tolower(sol[i]))-dir, 0)){
                    ac = false; break;
                }
            }
            if(puzzle.m_cnt) ac = false;
            if(ac){
                printf("Accepted: %d\n", n);
            }else puts("Wrong Answer");
            free(sol);
        }
        char tail[2];
        if(fgets(tail, 2, fpo)){
            puts("Output Limit Exceeded");
        }
    }
    if(fpi) fclose(fpi);
    if(fpo) fclose(fpo);
    return 0;
}
