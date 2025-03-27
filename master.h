#ifndef MASTER_H
#define MASTER_H

#include <raylib.h>
#include <raymath.h>
#include "../_slib/_slib.h"

static inline int _row_col_idx(int row, int col, int dim)
{
    return row * dim + col;
}

static inline int _idx_row(int idx, int dim)
{
    return idx / dim;
}

static inline int _idx_col(int idx, int dim)
{
    return idx % dim;
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



#endif