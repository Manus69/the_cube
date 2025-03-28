#include "Repr.h"

#define CYC_LEN 4

byte * _side_get(byte * side, int idx)
{
    return side + idx;
}

byte * _side_get_rc(byte * side, int row, int col)
{
    return _side_get(side, _row_col_idx(row, col));
}

void _side_row_to_buff(byte * restrict buff, byte * restrict side, int row)
{
    side = _side_get_rc(side, row, 0);
    for (int k = 0; k < DIM; k ++) buff[k] = side[k];
}

void _side_set_row(byte * restrict side, int row, byte * restrict buff)
{
    side = _side_get_rc(side, row, 0);
    for (int k = 0; k < DIM; k ++) side[k] = buff[k];
}

void _side_col_to_buff(byte * restrict buff, byte * restrict side, int col)
{
    for (int k = 0; k < DIM; k ++) buff[k] = * _side_get_rc(side, k, col);
}

void _side_set_col(byte * restrict side, int col, byte * restrict buff)
{
    for (int k = 0; k < DIM; k ++) * (byte *) _side_get_rc(side, k, col) = buff[k];
}

void _side_set_val(byte * side, byte val)
{
    for (int k = 0; k < DIM * DIM; k ++) * _side_get(side, k) = val;
}

void _side_set_rfc(byte * dst, int row, byte * src, int col)
{
    dst = _side_get_rc(dst, row, 0);
    for (int k = 0; k < DIM; k ++) dst[k] = * _side_get_rc(src, k, col);
}

void _side_set_rfr(byte * dst, int drow, byte * src, int srow)
{
    dst = _side_get_rc(dst, drow, 0);
    src = _side_get_rc(src, srow, 0);

    for (int k = 0; k < DIM; k ++) dst[k] = src[k];
}

void _side_set_cfr(byte * dst, int col, byte * src, int row)
{
    src = _side_get_rc(src, row, 0);
    for (int k = 0; k < DIM; k ++) * _side_get_rc(dst, k, col) = src[k]; 
}

byte * Repr_side(Repr * repr, CLR clr)
{
    return repr->buff + (clr * DIM * DIM);
}

byte * Repr_get(Repr * repr, CLR clr, int idx)
{
    return _side_get(Repr_side(repr, clr), idx);
}

static const CLR _move_side_seql[][4] =
{
    [CLR_R] = {CLR_Y, CLR_G, CLR_W, CLR_B},
    [CLR_Y] = {CLR_O, CLR_G, CLR_R, CLR_B},
};

void _cycle(Repr * repr, CLR const * sides, int const * idxs, int dir)
{
    byte buff;
    int k;

    buff = * Repr_get(repr, sides[0], 0);
    for (k = 0; k < CYC_LEN - 1; k ++)
    {
        * Repr_get(repr, sides[k], idxs[k]) = * Repr_get(repr, sides[k + 1], idxs[k + 1]);
    }

    * Repr_get(repr, sides[k], idxs[k]) = buff;
}

void _rot_r(Repr * repr, CLR clr)
{

    static const int _idx_map[CLR_$][3][4] = 
    {
        [CLR_R] = { {6, 0, 2, 8},
                    {7, 3, 1, 5},
                    {8, 6, 0, 2}},
        [CLR_Y] = { {2, 2, 2, 2},
                    {1, 1, 1, 1},
                    {0, 0, 0, 0},},
    };    

    _cycle(repr, _move_side_seql[clr], _idx_map[clr][0], 1);
    _cycle(repr, _move_side_seql[clr], _idx_map[clr][1], 1);
    _cycle(repr, _move_side_seql[clr], _idx_map[clr][2], 1);
}



static const int _corner_seql[] = {0, 2, 8, 6};
static const int _corner_seqr[] = {0, 6, 8, 2};
static const int _edge_seql[] = {1, 5, 7, 3};
static const int _edge_seqr[] = {1, 3, 7, 5};

void _side_rot(byte * side, int const * seq)
{
    byte buff;

    buff = side[seq[0]];
    for (int k = 0; k < 3; k ++)
    {
        side[seq[k]] = side[seq[k + 1]];
    }

    side[seq[3]] = buff;
}

void Repr_rot(Repr * repr, CLR clr, int dir)
{
    byte *  side;

    side = Repr_side(repr, clr);
    _side_rot(side, _corner_seql);
    _side_rot(side, _edge_seql);

    _rot_r(repr, clr);
}
