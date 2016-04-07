#include <cstdio>
#include <omp.h>

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
  bool isLegal[10];
  getPossibleValue(sudoku, isLegal, firstZero/9, firstZero%9, (firstZero/9)/3, (firstZero%9)/3);
  
# pragma omp parallel
  {
# pragma omp     for reduction (+ : numSolution ) firstprivate ( sudoku )
    for (int i = 1; i <= 9; i++) {
      if(isLegal[i]){
	sudoku [ firstZero / 9][ firstZero % 9] = i;
	numSolution += placeNumber ( nextn[firstZero/9][firstZero%9] , sudoku );
      }
    }
  }
  printf ("%d\n", numSolution );
  return 0;
}
