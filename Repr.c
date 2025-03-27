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

byte * Repr_side(Repr * repr, CLR clr)
{
    return repr->buff + (clr * repr->side_len * repr->side_len);
}

byte Repr_getv(Repr const * repr, CLR clr, int idx)
{
    return * _side_get(Repr_side((Repr *) repr, clr), idx);
}
