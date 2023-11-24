#include <cstdio>
#include <cstdlib>

int board[11][11];
int lib[11][11];

void printBoard(int b[11][11]){
  for(int i = 1; i <= 9; i++){
	for(int j = 1; j <= 9; j++)
	  printf("%d ", b[i][j]);
	printf("\n");
  }
}

void fc(int b[11][11], int original, int color, int x, int y){
  b[x][y] = color;
  if(b[x][y] == original)
	fc(b, original, color, x, )
	
	
}

void fillcolor(int b[11][11]){
  int color = 3;
  for(int i = 1; i <= 9; i++){
	for(int j = 1; j <= 9; j++)
	  if(b[i][j])
		fc(b, b[i][j], color++, i, j);
	  }
}

int main(){
  for(int i = 0; i < 11; i++)
	for(int j = 0; j < 11; j++)
	  board[i][j] = -1;
  for(int i = 1; i <= 9; i++)
	for(int j = 1; j <= 9; j++)
	  board[i][j] = rand()%3;//-1-boundary, 0-empty, 1-white, 2-black

  printf("board\n");  
  printBoard(board);

  fillcolor(board);
  
  int libi;
  for(int i = 1; i <= 9; i++)
	for(int j = 1; j <= 9; j++){
	  libi = 0;
	  if(board[i][j-1] == 0)
		libi++;
	  if(board[i][j+1] == 0)
		libi++;
	  if(board[i-1][j] == 0)
		libi++;
	  if(board[i+1][j] == 0)
		libi++;
	  lib[i][j] = libi;
	}

  printf("liberty\n");
  printBoard(lib);
}
