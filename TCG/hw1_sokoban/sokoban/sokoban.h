// by Yunghsien Chung (hiiragi4000)

// This file defines the structs needed and declares some functions.
// All functions declared in this file are defined in 'sokoban.c'.

#ifndef SOKOBAN_H
#define SOKOBAN_H

#include<stdbool.h>
#include<stdio.h>

#define HEIGHT_MAX 15
#define WIDTH_MAX 15
#define AREA_MAX 50
typedef struct{
    int height, width;
    char board[HEIGHT_MAX][WIDTH_MAX+2]; // +2 for LF and NULL characters
    // m_cnt: number of '$'s
    // the player is at (p_row, p_col)
    int m_cnt, p_row, p_col;
}Board;

// read a 'Board' from 'fin'
extern bool Get_Board(FILE *const fin, Board *const b_ptr);

// UP = 0, DOWN = 1, LEFT = 2, RIGHT = 3
typedef enum{
    UP, DOWN, LEFT, RIGHT
}Direction;

// push: whether a box is pushed in the move
typedef struct{
    Direction dir;
    bool push;
}Move;

// a dynamic array of 'Move', similar to 'std::vector<Move>' in c++
// use 'data[i]' to access the i-th element
// we have the access to 'data[0...capacity-1]'
// the data are stored in 'data[0...size-1]'
typedef struct{
    Move *data;
    int size, capacity;
}Vector_Move;

// initialize '*history'
extern void VM_init(Vector_Move *const history);

// append 'mv' to the back of '*history'
// similar to 'std::vector<Move>::push_back' in c++
extern void VM_push_back(Vector_Move *const history, Move const mv);

// append '(Move){dir, push}' to the back of '*history'
// similar to 'std::vector<Move>::emplace_back' in c++
extern void VM_emplace_back(Vector_Move *const history, Direction const dir, bool const push);

// remove the last element of '*history'
// similar to 'std::vector<Move>::pop_back' in c++
extern void VM_pop_back(Vector_Move *const history);

// make '*history' empty
// similar to 'std::vector<Move>::clear' in c++
extern void VM_clear(Vector_Move *const history);

// whether (i, j) is not outside the board '*b_ptr' and is not '#'
extern bool Inside(Board const *const b_ptr, int const i, int const j);

// make a move on the board '*b_ptr' in the direction 'dir'
// the function returns true iff 'dir' is valid
// if 'history' is nonNULL and 'dir' is valid, append the move to '*history'
extern bool Do_move(Board *const b_ptr, Direction const dir, Vector_Move *const history);

// given the last move 'mv', undo the board '*b_ptr'
// the function asserts the validity of 'mv'
extern void Undo(Board *const b_ptr, Move const mv);

#endif
