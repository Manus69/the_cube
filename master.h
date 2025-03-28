#ifndef MASTER_H
#define MASTER_H

#include <raylib.h>
#include <raymath.h>
#include "../_slib/_slib.h"

#define DIM 3

static inline int _row_col_idx(int row, int col)
{
    return row * DIM + col;
}

static inline int _idx_row(int idx)
{
    return idx / DIM;
}

static inline int _idx_col(int idx)
{
    return idx % DIM;
}

typedef enum
{
    CLR_R,
    CLR_G,
    CLR_O,
    CLR_B,
    CLR_Y,
    CLR_W,
    CLR_$,
}   CLR;

static inline CLR CLR_fromc(char x)
{
    static const CLR _tbl[] =
    {
        ['r'] = CLR_R,
        ['g'] = CLR_G,
        ['o'] = CLR_O,
        ['b'] = CLR_B,
        ['y'] = CLR_Y,
        ['w'] = CLR_W,
        ['_'] = CLR_$,
    };

    return _tbl[(int) x];
}

typedef struct
{
    char clr;
    char dir;
}   Cmd;

#endif