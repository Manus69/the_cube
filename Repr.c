#include "Repr.h"
#include <string.h>

#define CYC_LEN 4


void Repr_init(Repr * repr, char const * cstr)
{
    memcpy(repr->buff, cstr, REPR_LEN);
}

static byte * _side_get(byte * side, int idx)
{
    return side + idx;
}

byte * Repr_side(Repr * repr, CLR clr)
{
    return repr->buff + (clr * DIM * DIM);
}

byte * Repr_get(Repr * repr, CLR clr, int idx)
{
    return _side_get(Repr_side(repr, clr), idx);
}

byte * Repr_get_rc(Repr * repr, CLR clr, int row, int col)
{
    return Repr_get(repr, clr, _row_col_idx(row, col));
}

static void _cycle_rev(Repr * repr, CLR const * sides, int const * idxs)
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

static void _cycle(Repr * repr, CLR const * sides, int const * idxs, int dir)
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

u64 Repr_hash(Repr const * repr)
{
    return hash_djb(repr->buff, CLR_$ * DIM * DIM);
}

u64 Repr_hashf(void const * ptr)
{
    return Repr_hash(ptr);
}


int Repr_score(Repr const * repr)
{
    int score;

    score = 0;
    for (int k = 0; k < REPR_LEN; k ++)
    {
        score += (repr->buff[k] != CUBE_CLR_STR[k]); 
    }

    return score;
}

int Repr_score_dist(Repr const * repr)
{
    static const int clr_dist[CLR_$][CLR_$] =
    {
        {0, 1, 2, 1, 1, 1},
        {1, 0, 1, 2, 1, 1},
        {2, 1, 0, 1, 1, 1},
        {1, 2, 1, 0, 1, 1},
        {1, 1, 1, 1, 0, 2},
        {1, 1, 1, 1, 2, 0},
    };

    int score;
    // CLR clr;

    score = 0;
    for (int k = 0; k < REPR_LEN; k ++)
    {
        // clr = k % (DIM * DIM);
        score += clr_dist[CLR_fromc(CUBE_CLR_STR[k])][CLR_fromc(repr->buff[k])];
    }

    return score;
}

static int _score_row(byte const * row)
{
    if (row[0] == row[1] && row[1] == row[2] && row[2] == row[0]) return 0;
    if (row[0] == row[1]) return 1;
    if (row[1] == row[2]) return 1;

    return 2;
}

static int _score_rows(Repr const * repr, CLR clr)
{
    int     score;
    
    score = 0;
    
    for (int k = 0; k < DIM; k ++)
    {
        score += _score_row(Repr_get_rc((Repr *) repr, clr, k, 0));
    }

    return score;
}

int Repr_score_rows(Repr const * repr)
{
    int score;
    
    score = 0;
    for (CLR clr = 0; clr < CLR_$; clr ++)
    {
        score += _score_rows(repr, clr);   
    }

    return score;
}

static int _score_col(Repr const * repr, CLR clr, int col)
{
    byte buff[] = 
    {
        * Repr_get_rc((Repr *) repr, clr, 0, col), 
        * Repr_get_rc((Repr *) repr, clr, 1, col), 
        * Repr_get_rc((Repr *) repr, clr, 2, col),
    };

    return _score_row(buff);
}

static int _score_cols(Repr const * repr, CLR clr)
{
    int score;

    score = 0;
    for (int col = 0; col < DIM; col ++)
    {
        score += _score_col(repr, clr, col);
    }

    return score;
}

int Repr_score_cols(Repr const * repr)
{
    int score;

    score = 0;
    for (CLR clr = 0; clr < CLR_$; clr ++)
    {
        score += _score_cols(repr, clr);
    }

    return score;
}

int Repr_score_cum(Repr const * repr)
{
    return Repr_score_rows(repr) + Repr_score_cols(repr);
}

#include <stdio.h>

static void _dbg_side(Repr const * repr, CLR clr, char const * fmt)
{
    for (int row = 0; row < DIM; row ++)
    {
        for (int col = 0; col < DIM; col ++)
        {
            printf(fmt, * Repr_get((Repr *) repr, clr, _row_col_idx(row, col)));
        }
    }
    printf(" ");
}

void Repr_idx_dbg(Repr const * repr, CLR clr)
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

void Repr_clr_dbg(Repr const * repr)
{
    // printf("%.*s\n", REPR_LEN, (char *) repr->buff);
    for (CLR clr = 0; clr < CLR_$; clr ++)
    {
        _dbg_side(repr, clr, "%c");
    }
    $nl;
}
