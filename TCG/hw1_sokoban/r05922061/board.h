#pragma once

//including boundary(surrounding walls)
#define HEIGHT_MAX 17
#define WIDTH_MAX 17
#define AREA_MAX 50
#define MAX_DEPTH 200
#include <vector>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <map>

template <typename... Args> inline void epr(const char *s, Args... args) {
    fprintf(stderr, s, args...);
}

extern long long boardZobrist[HEIGHT_MAX][WIDTH_MAX];
extern long long floodFillZobrist[AREA_MAX];

//TODO: play and keep legalmove + region + box...



int const di[]={0, 1, 0, -1}, dj[]={-1, 0, 1, 0};
class PushMove{
public:
    PushMove(){}
PushMove(int boxi, int boxj, int dir): boxi(boxi), boxj(boxj), dir(dir){}
    int boxi, boxj;
    int dir;
    void print(){
        fprintf(stderr, "box at (%d,%d), dir at %c\n", boxi, boxj, "ldru"[dir]);
    }
};

struct Coordinate{
    int i, j;
};

class SearchInfo{
public:
    std::map<long long, int> closeData;
    std::vector<std::pair<int, int> > boxes;
    std::vector<std::pair<int, int> > goal;
};

class Board{
public:
    Board(){
        reset();
    }
    Board(FILE* const fin){
        reset();
        assert(getBoard(fin));
    }
    void reset(){
        epr("board reset\n");
        totalSearchNode = 0;
        finalDepth = 0;
        buildWall();
        history.clear();
        Board::info.closeData.clear();
    }
    void buildWall(){
        for(int i = 1; i <= height; i++){
            board[i][0] = '#';
            board[i][width+1] = '#';
        }
        for(int j = 1; j <= width; j++){
            board[0][j] = '#';
            board[height+1][j] = '#';
        }
    }

    void searchBFS();
    bool searchDFS(int depth);
    bool iterateDFS(int nowdepth, int depth);
    bool searchIDAstar(int threshold);
    int AStar(int g, int threshold);
    void printAnswer(Board original);
    bool operator<(Board& rhs){
        return f() < rhs.f();
        //https://stackoverflow.com/questions/19535644/how-to-use-the-priority-queue-stl-for-objects
    }
    int f();
    int g();
    int h();

    void print();
    void printHistory();
    void printRegion();
    void printLegalMoves();
    void printNextBoards();

    bool getBoard(FILE* const fin);
    bool play(PushMove& pm, int depth);
    bool undo(PushMove& pm);//or input nothing
    void genmove(int nowDepth = 0);
    long long getZobrist();


    bool isValid();
    bool isGoal(int i, int j);
    bool isWall(int i, int j);
    bool isBox(int i, int j);
    bool canFill(int i, int j);

    void buildBoxRegion();
    void updateRegion();
    void generateNextBoards();

    void changePlayerLocation(int newpi, int newpj);
    void fillRegion(int i, int j, int nowv);

    std::vector<char> getNonPushMoves(Board original);
    void generatePlayerPath(std::vector<char>& ansMoves, int afteri, int afterj);
    void fillPathDistance(int distance[HEIGHT_MAX][WIDTH_MAX], int i, int j, int d);

    int pi, pj;//player's coordinate
    int height, width;
    int goalCount;
    long long totalSearchNode;
    int finalDepth;

    char board[HEIGHT_MAX][WIDTH_MAX+2]; // +2 for LF and NULL characters
    int region[HEIGHT_MAX][WIDTH_MAX];
    static bool boxRegion[HEIGHT_MAX][WIDTH_MAX];//if true, box is valid in this place
    static SearchInfo info;

    std::vector<PushMove> legalMoves;
    std::vector<PushMove> history;
    std::vector< std::pair<int, Board> > nextBoards;
};

/*
class cmpAstarNode{
  public:
  bool operator()(const Board& lhs, const Board& rhs){
  return lhs.f() < rhs.f();
  }
  }
*/
