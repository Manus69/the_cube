#include "Repr.h"


byte * _side_get(byte * side, int idx)
{
    return side + idx;
}

byte * _side_get_rc(byte * side, int row, int col, int len)
{
    return _side_get(side, _row_col_idx(row, col, len));
}

void _side_cpy_row(byte * restrict dst, byte * restrict side, int row, int len)
{
    side = _side_get_rc(side, row, 0, len);
    for (int k = 0; k < len; k ++) dst[k] = side[k];
}

void _side_set_row(byte * restrict side, int row, byte * restrict buff, int len)
{
    side = _side_get_rc(side, row, 0, len);
    for (int k = 0; k < len; k ++) side[k] = buff[k];
}

void _side_cpy_col(byte * restrict dst, byte * restrict side, int col, int len)
{
    for (int k = 0; k < len; k ++)
    {
        dst[k] = * _side_get_rc(side, k, col, len);
    }
}

void _side_set_col(byte * restrict side, int col, byte * restrict buff, int len)
{
    for (int k = 0; k < len; k ++)
    {
        * (byte *) _side_get_rc(side, k, col, len) = buff[k];
    }
}

void _side_set_val(byte * side, int len, byte val)
{
    for (int k = 0; k < len; k ++)
    {
        * _side_get(side, k) = val;
    }
}

void _side_set_rfc(byte * restrict dst, int row, byte * restrict src, int col, int len)
{
    dst = _side_get_rc(dst, row, 0, len);
    for (int k = 0; k < len; k ++)
    {
        dst[k] = * _side_get_rc(src, k, col, len);
    }
}

void _side_set_rfr(byte * restrict dst, int drow, byte * restrict src, int srow, int len)
{
    dst = _side_get_rc(dst, drow, 0, len);
    src = _side_get_rc(src, srow, 0, len);

    for (int k = 0; k < len; k ++) dst[k] = src[k];
}

void _side_set_cfr(byte * restrict dst, int col, byte * restrict src, int row, int len)
{
    src = _side_get_rc(src, row, 0, len);
    for (int k = 0; k < len; k ++)
    {
        * _side_get_rc(dst, k, col, len) = src[k];
    }
}

byte * Repr_side(Repr * repr, CLR clr)
{
    return repr->buff + (clr * repr->side_len * repr->side_len);
}

byte Repr_getv(Repr const * repr, CLR clr, int idx)
{
    return * _side_get(Repr_side((Repr *) repr, clr), idx);
}

static const CLR _move_side_seq[][4] =
{
    [CLR_R] = {CLR_Y, CLR_B, CLR_W, CLR_G},
};

void Repr_rot(Repr * repr, CLR clr, int dir)
{
    byte *  side;
    byte    buff[1 << 6] = {};

    side = Repr_side(repr, _move_side_seq[CLR_R][0]);
    _side_cpy_row(buff, side, 2, 3);

    _side_set_rfc(side, 2, Repr_side(repr, CLR_G), 0, 3);
    _side_set_cfr(Repr_side(repr, CLR_G), 0, Repr_side(repr, CLR_W), 0, 3);
    _side_set_rfc(Repr_side(repr, CLR_W), 0, Repr_side(repr, CLR_B), 2, 3);
    _side_set_col(Repr_side(repr, CLR_B), 2, buff, 3);
}
