#ifndef MASTER_H
#define MASTER_H

#include <raylib.h>
#include <raymath.h>
// #include "../_slib/_slib.h"
#include "../_sflib/_sflib_cont.h"

#define DIM 3
#define CUBE_CLR_STR    "rrrrrrrrr" "ggggggggg" "ooooooooo" "bbbbbbbbb" "yyyyyyyyy" "wwwwwwwww"


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

static inline Cmd Cmd_fromc(char x)
{
    static const Cmd _cmds[] =
    {
        ['r'] = (Cmd) {CLR_R, 1},
        ['R'] = (Cmd) {CLR_R, -1},
        ['g'] = (Cmd) {CLR_G, 1},
        ['G'] = (Cmd) {CLR_G, -1},
        ['o'] = (Cmd) {CLR_O, 1},
        ['O'] = (Cmd) {CLR_O, -1},
        ['b'] = (Cmd) {CLR_B, 1},
        ['B'] = (Cmd) {CLR_B, -1},
        ['y'] = (Cmd) {CLR_Y, 1},
        ['Y'] = (Cmd) {CLR_Y, -1},
        ['w'] = (Cmd) {CLR_W, 1},
        ['W'] = (Cmd) {CLR_W, -1},
    };

    return _cmds[(int) x];
}

#endif