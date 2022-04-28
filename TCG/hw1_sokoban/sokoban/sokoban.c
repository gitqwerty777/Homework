// by Yunghsien Chung (hiiragi4000)

// This file defines the functions declared in 'sokoban.h'.
// You don't need to know much about how it works.

#include<assert.h>
#include<stdbool.h>
#include<stdio.h>
#include<stdlib.h>
#include"sokoban.h"

bool Get_Board(FILE *const fin, Board *const b_ptr){
    if(fscanf(fin, "%d%d ", &b_ptr->height, &b_ptr->width) != 2){
        return false;
    }
    b_ptr->m_cnt = 0;
    for(int i=0; i<=b_ptr->height-1; ++i){
        if(!fgets(b_ptr->board[i], WIDTH_MAX+2, fin)){
            assert(0);
        }
        b_ptr->board[i][b_ptr->width] = 0;
        for(int j=0; j<=b_ptr->width-1; ++j) switch(b_ptr->board[i][j]){
            case '@':
            case '+':
                b_ptr->p_row = i, b_ptr->p_col = j;
                break;
            case '$':
                ++b_ptr->m_cnt;
                break;
        }
    }
    return true;
}

void VM_init(Vector_Move *const history){
    history->data = 0;
    history->size = history->capacity = 0;
}
#define SOKOBAN_VM_ALLOC do{\
    if(history->size == history->capacity){\
        if(history->capacity == 0){\
            history->data = malloc(sizeof(Move));\
            history->capacity = 1;\
        }else{\
            history->data = realloc(history->data, 2*history->capacity*sizeof(Move));\
            history->capacity *= 2;\
        }\
    }\
}while(0)
void VM_push_back(Vector_Move *const history, Move const mv){
    SOKOBAN_VM_ALLOC;
    history->data[history->size++] = mv;
}
void VM_emplace_back(Vector_Move *const history, Direction const dir, bool const push){
    SOKOBAN_VM_ALLOC;
    history->data[history->size++] = (Move){dir, push};
}
#undef SOKOBAN_VM_ALLOC
void VM_pop_back(Vector_Move *const history){
    if(history->size-- == 0){
        assert(0);
    }
}
void VM_clear(Vector_Move *const history){
    free(history->data);
    history->data = 0;
    history->size = history->capacity = 0;
}

int const Di[]={-1, 1, 0, 0}, Dj[]={0, 0, -1, 1};
bool Inside(Board const *const b_ptr, int const i, int const j){
    return 0<=i && i<=b_ptr->height-1 && 0<=j && j<=b_ptr->width-1 && b_ptr->board[i][j]!='#';
}
bool Do_move(Board *const b_ptr, Direction const dir, Vector_Move *const history){
    if(!Inside(b_ptr, b_ptr->p_row+Di[dir], b_ptr->p_col+Dj[dir])){
        return false;
    }
    char *curr = &b_ptr->board[b_ptr->p_row][b_ptr->p_col], *next = &b_ptr->board[b_ptr->p_row+Di[dir]][b_ptr->p_col+Dj[dir]];
    // "@-", "@.", "+-", "+."
    if(*next=='-' || *next=='.'){
        *curr = (*curr=='@')? '-': '.';
        *next = (*next=='-')? '@': '+';
        b_ptr->p_row += Di[dir], b_ptr->p_col += Dj[dir];
        if(history){
            VM_emplace_back(history, dir, false);
        }
        return true;
    }
    // "@$?", "@*?", "+$?", "+*?"
    if(!Inside(b_ptr, b_ptr->p_row+2*Di[dir], b_ptr->p_col+2*Dj[dir])){
        return false;
    }
    char *nnext = &b_ptr->board[b_ptr->p_row+2*Di[dir]][b_ptr->p_col+2*Dj[dir]];
    if(*nnext=='$' || *nnext=='*'){
        return false;
    }
    *curr = (*curr=='@')? '-': '.';
    if(*next == '$'){
        *next = '@';
        --b_ptr->m_cnt;
    }else{
        *next = '+';
    }
    if(*nnext == '-'){
        *nnext = '$';
        ++b_ptr->m_cnt;
    }else{
        *nnext = '*';
    }
    b_ptr->p_row += Di[dir], b_ptr->p_col += Dj[dir];
    if(history){
        VM_emplace_back(history, dir, true);
    }
    return true;
}
void Undo(Board *const b_ptr, Move const mv){
    assert(Inside(b_ptr, b_ptr->p_row-Di[mv.dir], b_ptr->p_col-Dj[mv.dir]));
    char *curr = &b_ptr->board[b_ptr->p_row][b_ptr->p_col], *prev=&b_ptr->board[b_ptr->p_row-Di[mv.dir]][b_ptr->p_col-Dj[mv.dir]];
    if(*prev == '-'){
        *prev = '@';
    }else{
        assert(*prev == '.');
        *prev = '+';
    }
    if(mv.push){
        assert(Inside(b_ptr, b_ptr->p_row+Di[mv.dir], b_ptr->p_col+Dj[mv.dir]));
        char *next = &b_ptr->board[b_ptr->p_row+Di[mv.dir]][b_ptr->p_col+Dj[mv.dir]];
        if(*curr == '@'){
            *curr = '$';
            ++b_ptr->m_cnt;
        }else{
            *curr = '*';
        }
        if(*next == '$'){
            *next = '-';
            --b_ptr->m_cnt;
        }else{
            assert(*next == '*');
            *next = '.';
        }
    }else{
        *curr = (*curr=='@')? '-': '.';
    }
    b_ptr->p_row -= Di[mv.dir], b_ptr->p_col -= Dj[mv.dir];
}
