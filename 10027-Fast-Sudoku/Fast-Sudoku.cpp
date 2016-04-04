#include <cstdio>
#include <omp.h>

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

int placeNumber (int n, int sudoku [9][9]){
  if (n == 81)
    return 1;
  int row = n / 9;
  int col = n % 9;
  int blockRow = row / 3;
  int blockCol = col / 3;
  if ( sudoku [ row ][ col ] != 0)
    return ( placeNumber (n + 1, sudoku ));

  int numSolution = 0;
  bool isLegal[10];
  getPossibleValue(sudoku, isLegal, row, col, blockRow, blockCol);
  for (int tryi = 1; tryi <= 9; tryi ++) {
    if(!isLegal[tryi])
      continue;
    sudoku[row][col] = tryi;
    numSolution += placeNumber (n + 1, sudoku);
  } /* for */
  sudoku [row][col] = 0;
  return numSolution;
}
    
int main ( void ){
  int sudoku [9][9];
  int firstZero = -1;
  for (int i = 0; i < 9; i++)
    for (int j = 0; j < 9; j++) {
      scanf ("%d", &( sudoku [i][j ]));
      if ( sudoku [i][j] == 0 && firstZero == -1)
	firstZero = i * 9 + j;
    }
  omp_set_num_threads (9);
  int numSolution = 0;
# pragma omp parallel for reduction (+ : numSolution ) firstprivate ( sudoku ) 
  for (int i = 1; i <= 9; i++) {
    sudoku [ firstZero / 9][ firstZero % 9] = i;
    numSolution += placeNumber ( firstZero , sudoku );
  }
  printf ("%d\n", numSolution );
  return 0;
}
