#include "board.h"

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <climits>
#include <algorithm>
#include <queue>
#include"board.h"
int playcount = 0, undocount = 0;

bool Board::boxRegion[HEIGHT_MAX][WIDTH_MAX] = {{false}};
SearchInfo Board::info;

int reverseDirection[] = {2, 3, 0, 1}; //ldru -> ruld

long long boardZobrist[HEIGHT_MAX][WIDTH_MAX];
long long floodFillZobrist[AREA_MAX];

bool cmpMoveOrder(const std::pair<int, Board>& lhs, const std::pair<int, Board>& rhs){
    const Board& lb = lhs.second;
    const Board& rb = rhs.second;
    if(lb.goalCount != rb.goalCount){
        return lb.goalCount < rb.goalCount;
        //} else if (lb.getGoalDistance() != rb.getGoalDistance()){
    //return lb.getGoalDistance() < rb.getGoalDistance();
    } else {
        return false;
    }
}

void Board::searchBFS(){
    for(int depth=1; ; ++depth){
        if(searchDFS(depth)){
            epr("dfs in depth %d success, total search %lld\n", depth, totalSearchNode);
            epr("playcount %d undocount %d\n", playcount, undocount);
            return;
        }
    }
}

bool Board::searchDFS(int depth){
    epr("dfs in depth %d\n", depth);
    history.resize(depth);
    return iterateDFS(0, depth);
}

bool Board::iterateDFS(int nowdepth, int depth){
    if(nowdepth >= depth){
        return false;
    }
    if(goalCount == 0){//complete, no non-achieve goals(m_cnt)
        finalDepth = nowdepth;
        epr("find answer\n");
        print();
        return true;
    }
    if(!isValid()){
#ifdef DEBUG
        epr("invalid\n");
        print();
#endif
        return false;
    }

    updateRegion();//TODO: only need to get player's region
    genmove(nowdepth);
    if((Board::info.closeData.find(getZobrist()) != Board::info.closeData.end()) && (Board::info.closeData[getZobrist()] <= nowdepth)){
        return false;
    } else {
        Board::info.closeData[getZobrist()] = nowdepth;
    }
#ifdef DEBUG
    printLegalMoves();
#endif

    std::vector<PushMove> nowLegalMoves = this->legalMoves;
    //auto nowNextBoards = this->nextBoards;
    for(auto& pm: nowLegalMoves){
        //Board& nextBoard = it.second;//TODO: unused
        assert(play(pm, nowdepth));
        totalSearchNode++;
        if(iterateDFS(nowdepth+1, depth)){
            return true;
        } else {
            undo(pm);
            undocount++;
        }
    }
    return false;
}

//https://algorithmsinsight.wordpress.com/graph-theory-2/ida-star-algorithm-in-general/
#define ASTAR_FOUND -1
bool Board::searchIDAstar(int threshold){
    threshold = h();
    while(threshold < MAX_DEPTH){
        epr("astar search threshold %d\n", threshold);
        history.resize(threshold);
        //threshold will become larger after search
        int newThreshold = AStar(0, threshold);
        if(newThreshold == ASTAR_FOUND){
            return true;
        }
        threshold = newThreshold;
    }
    return false;
}

// Using lambda to compare elements.
//bool cmp(int left, int right) { return (left ^ 1) < (right ^ 1);}
int Board::AStar(int g, int threshold){//DFS_cost//g() = nowdepth, h() = heuristicDistance
    //std::priority_queue<Board, std::vector<Board>, cmp> pq;
    //std::priority_queue<Board, std::vector<Board>, cmpAstarNode> pq;//PQ is sort by f() = g()+h()
    if(goalCount == 0)               //Goal node found
        return ASTAR_FOUND;
    int f=g+h();
    if(f > threshold)//only search < threshold, report threshold
        return f;
    int min = INT_MAX;
    updateRegion();//TODO: only need to get player's region
    genmove(g);

    if((Board::info.closeData.find(getZobrist()) != Board::info.closeData.end()) && (Board::info.closeData[getZobrist()] <= f)){
        return false;
    } else {
        Board::info.closeData[getZobrist()] = f;
    }

    std::vector<PushMove> nowLegalMoves = this->legalMoves;
    for(auto& pm: nowLegalMoves){
        //recursive call with next node as current node for depth search
        play(pm, g);
        int temp = AStar(g+1, threshold);
        if(temp==ASTAR_FOUND)
            return temp;
        else if(temp<min)     //find the minimum of all ‘f’ greater than threshold encountered
            min=temp;
        undo(pm);
    }
    return min;  //return the minimum threshold
    /*std::priority_queue<Board, std::vector<Board>, cmpAstarNode> pq;//PQ is sort by f() = g()+h()
    pq.push(*this);
    while(!pq.empty()){
        Board b = pq.top();
        pq.pop();
        genmove(b.g());
        if((Board::info.closeData.find(b.getZobrist()) != Board::info.closeData.end()) && (Board::info.closeData[b.getZobrist()] <= b.g())){
            continue;
        } else {
            Board::info.closeData[getZobrist()] = b.g();
        }

        std::vector<PushMove> nowLegalMoves = b.legalMoves;
        auto nowNextBoards = b.nextBoards;
        for(auto& it: nowNextBoards){
            PushMove& pm = nowLegalMoves[it.first];
            Board& nextBoard = it.second;//TODO: unused
            //assert(b.play(pm, nowdepth));
            if(nextBoard.goalCount == 0)
                return true;
            pq.push(nextBoard);
            totalSearchNode++;
        }
        if(b.f() > threshold)
            continue;
    }
    return false;*/
}

int Board::f(){
    return g()+h();
}

int Board::g(){
    return 0;
}

int Board::h(){
    int dis = 0;
    for(int i = 1; i <= height; i++)
        for(int j = 1; j <= width; j++){
            if(isBox(i, j)){
                //for all goal position
                //find min distance
                //dis += minD;
            }
        }
    return dis;
}

/*
  bool Board::AStar(int threshold){//DFS_cost
  std::priority_queue<int, std::vector<int>, cmp> pq;
  pq.push(null, N0);//PQ is sort by f() = g()+h()
  while(!pq.empty()){
  (current, N) = pq.top();
  pq.pop();
  R = next1(current, N);
  genmove();
  pq.push(R, N);
  if(f(p) > threshold)
  continue;
  if(R is goal)
  return true;
  //If R is already in P Q, then continue//avoid loop

  pq.push(null, R);
  }
  return false;
  }
 */

void Board::genmove(int nowdepth){//at most box*4 possible moves
    legalMoves.clear();

    int nowRegion = region[pi][pj];
    for(int i = 1; i <= height; i++)
        for(int j = 1; j <= width; j++)
            if(nowRegion == region[i][j]){//get possible region to push
                for(int d = 0; d < 4; d++){//find boxes and check 4 directions to push it
                    if(isBox(i+di[d], j+dj[d]))
                        if(canFill(i+2*di[d], j+2*dj[d])){//if can push, add to legalMoves
#ifdef DEBUG
                            epr("add legal move from (%d, %d), dir %c\n", i, j, "ldru"[d]);
#endif
                            if((nowdepth >= 1) && (i+di[d] == history[nowdepth-1].boxi) && (j+di[d] == history[nowdepth-1].boxj) && (history[nowdepth-1].dir == reverseDirection[d]))//we don't want to reverse previous move
                                continue;
                            //if(isGoal(i+2*di[d], j+2*dj[d]))//simple move ordering: reach-goal move first
                            //  legalMoves.insert(legalMoves.begin(), PushMove(i+di[d], j+dj[d], d));
                            //else
                            legalMoves.push_back(PushMove(i+di[d], j+dj[d], d));
                        }
                }
            }

#ifdef DEBUG
    epr("before sort legalmoves\n");
    printLegalMoves();
#endif
    //generateNextBoards();
#ifdef DEBUG
    printNextBoards();
#endif
    //std::sort(nextBoards.begin(), nextBoards.end(), cmpMoveOrder);
#ifdef DEBUG
    epr("after sort legalmoves\n");
    printNextBoards();
#endif
}

void Board::generateNextBoards(){
    nextBoards.clear();
    int index = 0;
    for(auto& pm: legalMoves){
        Board b = *this;
        b.play(pm, 0);//depth is not important in this play()
        nextBoards.push_back(std::pair<int, Board>(index++, b));
    }
}

void Board::printNextBoards(){
    for(auto& nb: nextBoards){
        epr("move index %d\n", nb.first);
        nb.second.print();
    }
}

void Board::printLegalMoves(){
    epr("legal moves\n");
    for(auto& pm: legalMoves){
        pm.print();
    }
}

void Board::updateRegion(){
    for(int i = 0; i <= height+1; i++){
        for(int j = 0; j <= width+1; j++){
            region[i][j] = 0;
        }
    }
    int nowv = 1;
    for(int i = 1; i <= height; i++){
        for(int j = 1; j <= width; j++){
            if(canFill(i, j) && (region[i][j] == 0)){
                fillRegion(i, j, nowv);
                nowv++;
            }
        }
    }
    printRegion();
}


void Board::fillRegion(int i, int j, int nowv){
    if(canFill(i, j) && (region[i][j] == 0)){
        region[i][j] = nowv;
        for(int d = 0; d < 4; d++){
            fillRegion(i+di[d], j+dj[d], nowv);
        }
    }
}

void Board::printRegion(){
#ifdef DEBUG
    epr("print region\n");
    for(int i = 1; i <= height; i++){
        for(int j = 1; j <= width; j++){
            epr("%d ", region[i][j]);
        }
        epr("\n");
    }
#endif
}

void Board::changePlayerLocation(int newpi, int newpj){
    if(board[pi][pj] == '@'){
        board[pi][pj] = '-';
    } else if(board[pi][pj] == '+'){
        board[pi][pj] = '.';
    } else {
        assert(false);
    }
    pi = newpi; pj = newpj;
    if(board[pi][pj] == '-'){
        board[pi][pj] = '@';
    } else if(board[pi][pj] == '.'){
        board[pi][pj] = '+';
    } else {
        assert(false);
    }
}

bool Board::play(PushMove& pm, int nowdepth){
#ifdef DEBUG
    epr("================================\nbefore play\n");
    print();
#endif

    int dir = pm.dir;
    int bi = pm.boxi;
    int bj = pm.boxj;
    int pi = pm.boxi-di[dir];
    int pj = pm.boxj-dj[dir];

    assert(!isWall(pi, pj));
    changePlayerLocation(pi, pj);
    history[nowdepth] = pm;

    char *curr = &board[pi][pj], *next = &board[bi][bj];
    char *nnext = &board[pi+2*di[dir]][pj+2*dj[dir]];
    *curr = (*curr=='@')? '-': '.';
    if(*next == '$'){
        *next = '@';
        --goalCount;
    } else{
        *next = '+';
    }
    if(*nnext == '-'){
        *nnext = '$';
        ++goalCount;
    } else{
        *nnext = '*';
    }
    this->pi = bi; this->pj = bj;
#ifdef DEBUG
    epr("after play\n");
    pm.print();
    print();
    epr("================================\n");
#endif
    return true;
}
bool Board::undo(PushMove& pm){//or input nothing
    int dir = pm.dir;
    int pi = pm.boxi;
    int pj = pm.boxj;
    changePlayerLocation(pi, pj);
#ifdef DEBUG
    int bi = pm.boxi+di[dir];
    int bj = pm.boxj+dj[dir];
    epr("undo move box %d,%d player %d,%d prev %d,%d\n", bi, bj, pi, pj, pi-di[dir], pj-dj[dir]);
    pm.print();
    print();
    assert(!isWall(pi-di[dir], pj-dj[dir]));
#endif

    char *curr = &board[pi][pj], *prev=&board[pi-di[dir]][pj-dj[dir]];
    if(*prev == '-'){
        *prev = '@';
    }else{
        assert(*prev == '.');
        *prev = '+';
    }

    char *next = &board[pi+di[dir]][pj+dj[dir]];
    if(*curr == '@'){
        *curr = '$';
        ++goalCount;
    }else{
        *curr = '*';
    }
    if(*next == '$'){
        *next = '-';
        --goalCount;
    }else{
        assert(*next == '*');
        *next = '.';
    }
    this->pi = pi-di[dir]; this->pj = pj-dj[dir];

#ifdef DEBUG
    epr("after undo move\n");
    print();
#endif
    return true;
}

bool Board::getBoard(FILE* const fin){
    if(fscanf(fin, "%d%d ", &height, &width) != 2){
        return false;
    }
    goalCount = 0;
    for(int i=1; i<=height; ++i){
        if(!fgets(&board[i][1], WIDTH_MAX+2, fin)){
            assert(0);
        }
        board[i][width+1] = 0;
        for(int j=1; j<=width; ++j) switch(board[i][j]){
            case '@':
            case '+':
                pi = i, pj = j;
                break;
            case '$':
                ++goalCount;
                break;
            }
    }
    buildWall();
    buildBoxRegion();
    return true;
}

void Board::buildBoxRegion(){
    //generate a bool array that shows box in this grid is invalid or not
    //for each non-wall grid
    //1. if no wall neighbor: true
    //2. if one wall: check it's wall, if it can escape or meet goal: true; otherwise: false
    //3. if corner(two continuous wall) and it's not goal: false

    bool cornerRegion[HEIGHT_MAX][WIDTH_MAX] = {{false}};
    int oneWall[HEIGHT_MAX][WIDTH_MAX] = {{0}};
    for(int i = 1; i <= height; i++)
        for(int j = 1; j <= width; j++)
            Board::boxRegion[i][j] = false;

    for(int i = 1; i <= height; i++){
        for(int j = 1; j <= width; j++){
            if(isWall(i, j))
                continue;
            bool wallCount[4] = {false};
            for(int d = 0; d < 4; d++){
                if(isWall(i+di[d], j+dj[d]))
                    wallCount[d] = true;
            }
            bool noWall = true;
            bool isCorner = false;
            int wallCounts = 0;
            for(int d = 0; d < 4; d++){
                if(wallCount[d]){
                    noWall = false;
                    wallCounts++;
                    oneWall[i][j] = d+1;
                }
                if(wallCount[d] && wallCount[(d+1)%4])
                    isCorner = true;
            }
            if(wallCounts != 1)
                oneWall[i][j] = 0;
            if(noWall) {
                epr("%d, %d nowall\n", i, j);
                Board::boxRegion[i][j] = true;
            } else if(isGoal(i, j)){
                epr("%d, %d isgoal\n", i, j);
                Board::boxRegion[i][j] = true;
                oneWall[i][j] = 0;
            } else {
                if (isCorner){
                    epr("%d, %d not-goal, iscorner\n", i, j);
                    cornerRegion[i][j] = true;
                } else {
                    Board::boxRegion[i][j] = true;
                }
            }
        }
    }
    epr("final corner region\n");
    for(int i = 1; i <= height; i++){
        for(int j = 1; j <= width; j++){
            epr("%d ", (cornerRegion[i][j])?1:0);
        }
        epr("\n");
    }
    for(int i = 1; i <= height; i++){
        for(int j = 1; j <= width; j++){
            if(oneWall[i][j] != 0){
                epr("onewall %d,%d=%d\n", i, j, oneWall[i][j]);
                if(oneWall[i][j] == 1){//l
                    bool escapeWall = false;
                    bool meetGoal = false;
                    for(int ii = i; ii <= height; ii++){
                        if(cornerRegion[ii][j])
                            break;
                        if(!isWall(ii, j-1))
                            escapeWall = true;
                        if(isGoal(ii, j))
                            meetGoal = true;
                    }
                    for(int ii = i; ii >= 1; ii--){
                        if(cornerRegion[ii][j])
                            break;
                        if(!isWall(ii, j-1))
                            escapeWall = true;
                        if(isGoal(ii, j))
                            meetGoal = true;
                    }
                    boxRegion[i][j] = (escapeWall || meetGoal);
                } else if(oneWall[i][j] == 2){//d
                    bool escapeWall = false;
                    bool meetGoal = false;
                    for(int jj = j; jj <= width; jj++){
                        if(cornerRegion[i][jj])
                            break;
                        if(!isWall(i+1, jj))
                            escapeWall = true;
                        if(isGoal(i, jj))
                            meetGoal = true;
                    }
                    for(int jj = j; jj >= 0; jj--){
                        if(cornerRegion[i][jj])
                            break;
                        if(!isWall(i+1, jj))
                            escapeWall = true;
                        if(isGoal(i, jj))
                            meetGoal = true;
                    }
                    boxRegion[i][j] = (escapeWall || meetGoal);
                } else if(oneWall[i][j] == 3){//r
                    bool escapeWall = false;
                    bool meetGoal = false;
                    for(int ii = i; ii <= height; ii++){
                        if(cornerRegion[ii][j])
                            break;
                        if(!isWall(ii, j+1))
                            escapeWall = true;
                        if(isGoal(ii, j))
                            meetGoal = true;
                    }
                    for(int ii = i; ii >= 1; ii--){
                        if(cornerRegion[ii][j])
                            break;
                        if(!isWall(ii, j+1))
                            escapeWall = true;
                        if(isGoal(ii, j))
                            meetGoal = true;
                    }
                    boxRegion[i][j] = (escapeWall || meetGoal);
                } else if(oneWall[i][j] == 4){//u
                    bool escapeWall = false;
                    bool meetGoal = false;
                    for(int jj = j; jj <= width; jj++){
                        if(cornerRegion[i][jj])
                            break;
                        if(!isWall(i-1, jj))
                            escapeWall = true;
                        if(isGoal(i-1, jj))
                            meetGoal = true;
                    }
                    for(int jj = j; jj >= 0; jj--){
                        if(cornerRegion[i][jj])
                            break;
                        if(!isWall(i-1, jj))
                            escapeWall = true;
                        if(isGoal(i-1, jj))
                            meetGoal = true;
                    }
                    boxRegion[i][j] = (escapeWall || meetGoal);
                }
                epr("after onewall %d,%d= %d\n", i, j, boxRegion[i][j]);
            }
        }
    }
    for(int i = 1; i <= height; i++){
        for(int j = 1; j <= width; j++){
            if(cornerRegion[i][j]){
                epr("%d, %d not-goal, iscorner\n", i, j);
                Board::boxRegion[i][j] = false;
            }
        }
    }
    epr("box region\n");
    print();
    for(int i = 1; i <= height; i++){
        for(int j = 1; j <= width; j++){
            epr("%d ", (Board::boxRegion[i][j])?1:0);
        }
        epr("\n");
    }
}
inline bool Board::isGoal(int i, int j){
    return ((board[i][j] == '*') || (board[i][j] == '+') ||  (board[i][j] == '.'));
}
inline bool Board::isBox(int i, int j){
    return (board[i][j] == '$') || (board[i][j] == '*');
}
inline bool Board::isWall(int i, int j){
    return board[i][j]=='#';
}
bool Board::canFill(int i, int j){
    return (board[i][j] != '#') && (board[i][j] != '$') && (board[i][j] != '*');
}

void Board::print(){
    epr("player at (%d, %d), goalCount %d\n", pi, pj, goalCount);

    for(int i = 1; i <= height; i++){
        for(int j = 1; j <= width; j++){
            epr("%c", board[i][j]);
        }
        epr("\n");
    }
}

bool Board::isValid(){
    for(int i = 1; i <= height; i++){
        for(int j = 1; j <= width; j++){
            if(board[i][j] == '$'){
                if(!Board::boxRegion[i][j])
                    return false;
            }
        }
    }
    return true;
}

void Board::printHistory(){
#ifdef DEBUG
    epr("history\n");
    for(auto& pm: history)
        pm.print();
#endif
}

std::vector<char> Board::getNonPushMoves(Board original){
    std::vector<char> ansMoves;
    int afteri, afterj;
    Board nowb = original;
    epr("original board");
    nowb.print();
    nowb.history.resize(1);
    history.resize(finalDepth);
    for(auto& pm: history){
        afteri = pm.boxi-di[pm.dir];
        afterj = pm.boxj-dj[pm.dir];
        nowb.generatePlayerPath(ansMoves, afteri, afterj);
        ansMoves.push_back("LDRU"[pm.dir]);
        nowb.print();
        nowb.play(pm, 0);
        nowb.print();
    }
    return ansMoves;
}

void Board::generatePlayerPath(std::vector<char>& ansMoves, int afteri, int afterj){
    int distance[HEIGHT_MAX][WIDTH_MAX] = {{0}};
    for(int i = 0; i <= height+1; i++)
        for(int j = 0; j <= width+1; j++){
            if(isBox(i, j) || isWall(i, j))
                distance[i][j] = -1;
            else
                distance[i][j] = AREA_MAX;
        }
    //do BFS to get shortest path
    // fill shortest path distance
    fillPathDistance(distance, pi, pj, 0);
    print();
    epr("distance\n");
    for(int i = 0; i <= height+1; i++){
        for(int j = 0; j <= width+1; j++){
            epr("%02d ", distance[i][j]);
        }
        epr("\n");
    }
    std::vector<char> insertMoves;
    epr("backtrack %d, %d to %d, %d\n", afteri, afterj, pi, pj);
    int nowi = afteri, nowj = afterj;
    while((nowi != pi) || (nowj != pj)){
        for(int d = 0; d < 4; d++){
            if((distance[nowi+di[d]][nowj+dj[d]] != -1) &&
               (distance[nowi+di[d]][nowj+dj[d]] == distance[nowi][nowj]-1)){
                nowi = nowi+di[d];
                nowj = nowj+dj[d];
                insertMoves.push_back("ldru"[reverseDirection[d]]);
                break;
            }
        }
    }
    epr("total %d non push moves\n", insertMoves.size());
    std::reverse(insertMoves.begin(), insertMoves.end());
    ansMoves.insert(ansMoves.end(), insertMoves.begin(), insertMoves.end());
}

void Board::fillPathDistance(int distance[HEIGHT_MAX][WIDTH_MAX], int i, int j, int d){
    if(distance[i][j] == -1)
        return;
    if(d < distance[i][j]){
        distance[i][j] = d;
        for(int dir = 0; dir < 4; dir++){
            fillPathDistance(distance, i+di[dir], j+dj[dir], d+1);
        }
    }
}

void Board::printAnswer(Board original){
    std::vector<char> ansMoves = getNonPushMoves(original);
    epr("print non-push moves(%d)\n", ansMoves.size());
    printf("%lu\n", ansMoves.size());
    for(char c: ansMoves)
        printf("%c", c);
    printf("\n");
}

long long Board::getZobrist(){
    long long zobrist = 0;
    for(int i = 1; i <= height; i++)
        for(int j = 1; j <= width; j++){
            if(isBox(i, j)){
                zobrist ^= boardZobrist[i][j];
            }
        }
    //epr("board zobrist %lld\n", zobrist);
    //printRegion();
    int regionIndex = region[pi][pj];
    //epr("region index of %d, %d = %d\n", pi, pj, regionIndex);
    zobrist ^= floodFillZobrist[regionIndex-1];//0~49
    //epr("final zobrist %lld\n", zobrist);
    return zobrist;
}
