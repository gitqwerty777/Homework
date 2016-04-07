#include <cstdio>
#include <omp.h>
#include <stack>

int nextn[9][9];

inline void getPossibleValue(int sudoku[9][9], bool isLegal[], int row, int col, int blockRow, int blockCol){
  for(int i = 1; i <= 9; i ++)
    isLegal[i] = true;
  for (int i = 0; i < 9; i++){
    isLegal[sudoku[row][i]] = false;
    isLegal[sudoku[i][col]] = false;
  }
  for (int i = 0; i < 3; i ++)
    for (int j = 0; j < 3; j ++)
      isLegal[sudoku[3*blockRow+i][3*blockCol+j]] = false;
}

inline int getNextIndex(int n, int sudoku[9][9]){
  n = n+1;
  while((n < 81) && (sudoku[n/9][n%9] != 0)){
    n++;
  }
  return n;
}

int placeNumber (int n, int sudoku [9][9]){
  if (n == 81)
    return 1;
  int row = n / 9;
  int col = n % 9;
  int blockRow = row / 3;
  int blockCol = col / 3;

  int numSolution = 0;
  bool isLegal[10];
  getPossibleValue(sudoku, isLegal, row, col, blockRow, blockCol);
  for (int tryi = 1; tryi <= 9; tryi ++) {
    if(!isLegal[tryi])
      continue;
    sudoku[row][col] = tryi;
    numSolution += placeNumber (nextn[row][col], sudoku);
  } /* for */
  sudoku [row][col] = 0;
  return numSolution;
}

class Board{
public:
  Board(int s[9][9], int i){
    std::copy(&s[0][0], &s[0][0] + 81, &sudoku[0][0]);
    nowi = i;
  }

  Board(int s[9][9], int i, int r, int c, int v){
    std::copy(&s[0][0], &s[0][0] + 81, &sudoku[0][0]);
    sudoku[r][c] = v;
    nowi = i;
  }
  void print(){
    for(int i = 0; i < 9; i ++){
      for(int j = 0; j < 9; j++){
	fprintf(stderr, "%d ",  sudoku[i][j]);
      }
      fprintf(stderr, "\n");
    }
  }
  int sudoku[9][9];
  int nowi;
};




int main ( void ){
  int sudoku [9][9];
  int firstZero = -1;
  int preZero = -1;
  for (int i = 0; i < 9; i++)
    for (int j = 0; j < 9; j++) {
      scanf ("%d", &( sudoku [i][j ]));
      if ( sudoku [i][j] == 0)
	if(firstZero == -1){
	  firstZero = i * 9 + j;
	  preZero = firstZero;
	} else {
	  nextn[preZero/9][preZero%9] = i * 9 + j;
	  preZero = i * 9 + j;
	}
    }
  if(preZero != -1)
    nextn[preZero/9][preZero%9] = 81;
  omp_set_num_threads (9);
  int numSolution = 0;
  //bool isLegal[10];
  //getPossibleValue(sudoku, isLegal, firstZero/9, firstZero%9, (firstZero/9)/3, (firstZero%9)/3);
  
  Board b(sudoku, firstZero);
  std::stack<Board> s;
  while(!s.empty())
    s.pop();
  s.push(b);

  
  while(!s.empty()){
    Board nowb = s.top();
    //fprintf(stderr, "nown = %d\n", nowb.nowi);
    //nowb.print();
    s.pop();
    if(nowb.nowi == 81){
      numSolution += 1;
      continue;
    }
    bool isLegal[10];
    getPossibleValue(nowb.sudoku, isLegal, nowb.nowi/9, nowb.nowi%9, (nowb.nowi/9)/3, (nowb.nowi%9)/3);
    int nexti = nextn[nowb.nowi/9][nowb.nowi%9];

# pragma omp parallel for firstprivate(nexti, isLegal)
    for (int i = 1; i <= 9; i++) {
      if(isLegal[i]){
	//fprintf(stderr, "push next n =  %d, nowi = %d\n", nextn[nowb.nowi/9][nowb.nowi%9], nowb.nowi);
	Board nextb = Board(nowb.sudoku, nexti, nowb.nowi/9, nowb.nowi%9, i);
	#pragma omp critical
	s.push(nextb);
      }

  }
}

  printf ("%d\n", numSolution );
  return 0;
}
