#include <cstdio>
#include "board.h"

void readZobrist(){
    FILE* f = fopen("sokobanHash.txt", "r");
    for(int i = 1; i < HEIGHT_MAX-1; i++)
        for(int j = 1; j < WIDTH_MAX-1; j++){
            assert(fscanf(f, "%lld", &boardZobrist[i][j]) == 1);//total 15*15 = 225
        }
    for(int i = 0; i < AREA_MAX; i++){
        assert(fscanf(f, "%lld", &floodFillZobrist[i]) == 1);//total 50
    }
    fclose(f);
}

int main(){
    readZobrist();
    Board b;
    while(b.getBoard(stdin)){
        Board originalBoard = b;
        epr("now height %d width %d\n", b.height, b.width);
        b.print();
        if(b.searchDFS(MAX_DEPTH))
            epr("dfs in depth %d success, total search %lld\n", b.finalDepth, b.totalSearchNode);
        else
            epr("dfs in depth %d failed\n", MAX_DEPTH);
        b.printAnswer(originalBoard);
        b.reset();
    }
}
