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

void _cycle_rev(Repr * repr, CLR const * sides, int const * idxs)
{
    byte    buff;
    int     k;

    buff = * Repr_get(repr, sides[0], idxs[0]);
    for (k = 0; k < CYC_LEN - 1; k ++)
    {
        * Repr_get(repr, sides[k], idxs[k]) = * Repr_get(repr, sides[k + 1], idxs[k + 1]);
    }

    * Repr_get(repr, sides[k], idxs[k]) = buff;
}

void _cycle(Repr * repr, CLR const * sides, int const * idxs, int dir)
{
    byte    buff;
    int     k;

    if (dir < 0) return _cycle_rev(repr, sides, idxs);

    buff = * Repr_get(repr, sides[CYC_LEN - 1], idxs[CYC_LEN - 1]);
    for (k = CYC_LEN - 1; k > 0; k --)
    {
        * Repr_get(repr, sides[k], idxs[k]) = * Repr_get(repr, sides[k - 1], idxs[k - 1]);
    }

    * Repr_get(repr, sides[k], idxs[k]) = buff;
}

static const int _face_rot_data[2][CYC_LEN] =
{
    {0, 6, 8, 2},
    {1, 3, 7, 5},
};

static const CLR _clr_rot_data[CLR_$][2][CYC_LEN] =
{
    [CLR_R][0] = {CLR_R, CLR_R, CLR_R, CLR_R},
    [CLR_R][1] = {CLR_Y, CLR_B, CLR_W, CLR_G},

    [CLR_Y][0] = {CLR_Y, CLR_Y, CLR_Y, CLR_Y},
    [CLR_Y][1] = {CLR_R, CLR_G, CLR_O, CLR_B},

    [CLR_G][0] = {CLR_G, CLR_G, CLR_G, CLR_G},
    [CLR_G][1] = {CLR_Y, CLR_R, CLR_W, CLR_O},

    [CLR_O][0] = {CLR_O, CLR_O, CLR_O, CLR_O},
    [CLR_O][1] = {CLR_Y, CLR_G, CLR_W, CLR_B},

    [CLR_B][0] = {CLR_B, CLR_B, CLR_B, CLR_B},
    [CLR_B][1] = {CLR_Y, CLR_O, CLR_W, CLR_R},

    [CLR_W][0] = {CLR_W, CLR_W, CLR_W, CLR_W},
    [CLR_W][1] = {CLR_R, CLR_B, CLR_O, CLR_G},
};

static const int _idx_rot_data[CLR_$][3][CYC_LEN] =
{
    [CLR_R][0] = {6, 8, 2, 0},
    [CLR_R][1] = {7, 5, 1, 3},
    [CLR_R][2] = {8, 2, 0, 6},

    [CLR_Y][0] = {0, 0, 0, 0},
    [CLR_Y][1] = {1, 1, 1, 1},
    [CLR_Y][2] = {2, 2, 2, 2},
    
    [CLR_G][0] = {8, 8, 8, 0},
    [CLR_G][1] = {5, 5, 5, 3},
    [CLR_G][2] = {2, 2, 2, 6},

    [CLR_O][0] = {0, 2, 8, 6},
    [CLR_O][1] = {1, 5, 7, 3},
    [CLR_O][2] = {2, 8, 6, 0},

    [CLR_B][0] = {0, 8, 0, 0},
    [CLR_B][1] = {3, 5, 3, 3},
    [CLR_B][2] = {6, 2, 6, 6},

    [CLR_W][0] = {6, 6, 6, 6},
    [CLR_W][1] = {7, 7, 7, 7},
    [CLR_W][2] = {8, 8, 8, 8},
};

void Repr_rot(Repr * repr, CLR clr, int dir)
{
    _cycle(repr, _clr_rot_data[clr][0], _face_rot_data[0], dir);
    _cycle(repr, _clr_rot_data[clr][0], _face_rot_data[1], dir);

    _cycle(repr, _clr_rot_data[clr][1], _idx_rot_data[clr][0], dir);
    _cycle(repr, _clr_rot_data[clr][1], _idx_rot_data[clr][1], dir);
    _cycle(repr, _clr_rot_data[clr][1], _idx_rot_data[clr][2], dir);
}

// void _rot_r(Repr * repr, CLR clr, int dir)
// {
//     _cycle(repr, _clr_rot_data[clr][0], _face_rot_data[0], dir);
//     _cycle(repr, _clr_rot_data[clr][0], _face_rot_data[1], dir);

//     _cycle(repr, _clr_rot_data[clr][1], _idx_rot_data[clr][0], dir);
//     _cycle(repr, _clr_rot_data[clr][1], _idx_rot_data[clr][1], dir);
//     _cycle(repr, _clr_rot_data[clr][1], _idx_rot_data[clr][2], dir);
// }

// void _rot_y(Repr * repr, CLR clr, int dir)
// {
//     _cycle(repr, (CLR []){CLR_Y, CLR_Y, CLR_Y, CLR_Y}, (int []){0, 6, 8, 2}, dir);
//     _cycle(repr, (CLR []){CLR_Y, CLR_Y, CLR_Y, CLR_Y}, (int []){1, 3, 7, 5}, dir);

//     _cycle(repr, (CLR []){CLR_R, CLR_G, CLR_O, CLR_B}, (int []){0, 0, 0, 0}, dir);
//     _cycle(repr, (CLR []){CLR_R, CLR_G, CLR_O, CLR_B}, (int []){1, 1, 1, 1}, dir);
//     _cycle(repr, (CLR []){CLR_R, CLR_G, CLR_O, CLR_B}, (int []){2, 2, 2, 2}, dir);
// }



// static const int _corner_seql[] = {0, 2, 8, 6};
// static const int _corner_seqr[] = {0, 6, 8, 2};
// static const int _edge_seql[] = {1, 5, 7, 3};
// static const int _edge_seqr[] = {1, 3, 7, 5};

// void _side_rot(byte * side, int const * seq)
// {
//     byte buff;

//     buff = side[seq[0]];
//     for (int k = 0; k < 3; k ++)
//     {
//         side[seq[k]] = side[seq[k + 1]];
//     }

//     side[seq[3]] = buff;
// }



#include <stdio.h>

void Repr_dbg(Repr const * repr, CLR clr)
{
    for (int row = 0; row < DIM; row ++)
    {
        for (int col = 0; col < DIM; col ++)
        {
            printf("%u ", * Repr_get((Repr *) repr, clr, _row_col_idx(row, col)));
        }
        $nl;
    }
    $nl;
}
