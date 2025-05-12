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

byte Repr_getv(Repr const * repr, CLR clr, int idx)
{
    return (repr->buff + (clr * DIM * DIM))[idx];
}

byte Repr_getv_rc(Repr const * repr, CLR clr, int row, int col)
{
    return Repr_getv(repr, clr, _row_col_idx(row, col));
}

bool Repr_is_clr(Repr const * repr, CLR side, int idx, CLR clr)
{
    return CLR_fromc(Repr_getv(repr, side, idx)) == clr;
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
    return sfl_hash(repr->buff, CLR_$ * DIM * DIM);
}

u64 Repr_hashf(void const * ptr)
{
    return Repr_hash(ptr);
}

int Repr_score_diff(Repr const * repr, byte const * rhs)
{
    int score;

    score = 0;
    for (int k = 0; k < REPR_LEN; k ++)
    {
        score += (repr->buff[k] != rhs[k]);
    }

    return score;
}

int Repr_score(Repr const * repr)
{
    return Repr_score_diff(repr, (byte const *) CUBE_CLR_STR);
}

int Repr_score_dist(Repr const * repr, byte const * rhs)
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

    score = 0;
    for (int k = 0; k < REPR_LEN; k ++)
    {
        score += clr_dist[CLR_fromc(rhs[k])][CLR_fromc(repr->buff[k])];
    }

    return score;
}

static int _score_row(byte const * row)
{
    if (row[0] == row[1] && row[1] == row[2] && row[2] == row[0]) return 0; //
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

static int _score_block(Repr const * repr, CLR clr, int row, int col)
{
    return  !  (Repr_getv_rc(repr, clr, row, col) == Repr_getv_rc(repr, clr, row, col + 1) &&
                Repr_getv_rc(repr, clr, row, col) == Repr_getv_rc(repr, clr, row + 1, col) &&
                Repr_getv_rc(repr, clr, row, col) == Repr_getv_rc(repr, clr, row + 1, col + 1));
}

static int _score_blocks(Repr const * repr, CLR clr)
{
    int score;

    score = 0;
    score += _score_block(repr, clr, 0, 0);
    score += _score_block(repr, clr, 0, 1);
    score += _score_block(repr, clr, 1, 0);
    score += _score_block(repr, clr, 1, 1);

    return score;
}

int Repr_score_blocks(Repr const * repr)
{
    int score;

    score = 0;
    for (CLR clr = 0; clr < CLR_$; clr ++)
    {
        score += _score_blocks(repr, clr);
    }

    return score;
}

int Repr_score_cum(Repr const * repr)
{
    return Repr_score_rows(repr) + Repr_score_cols(repr);
}

static int _score_rod(byte const * lhs, byte const * rhs)
{
    int     score;

    score = 0;

    score += ! (lhs[0] == lhs[1] && rhs[0] == rhs[1]);
    score += ! (lhs[2] == lhs[1] && rhs[2] == rhs[1]);

    return score;
}

static int _fill_rod(Repr const * repr, byte * lhs, CLR lhs_clr, int lhs_idx[3], byte * rhs, CLR rhs_clr, int rhs_idx[3])
{
    for (int k = 0; k < DIM; k ++)
    {
        lhs[k] = Repr_getv(repr, lhs_clr, lhs_idx[k]);
        rhs[k] = Repr_getv(repr, rhs_clr, rhs_idx[k]);
    }

    return _score_rod(lhs, rhs);
}

int Repr_score_rod(Repr const * repr)
{
    byte lhs[3] = {};
    byte rhs[3] = {};
    
    int score;

    score = 0;
    score += _fill_rod(repr, lhs, CLR_B, (int []) {2, 5, 8}, rhs, CLR_R, (int []) {0, 3, 6});
    score += _fill_rod(repr, lhs, CLR_R, (int []) {2, 5, 8}, rhs, CLR_G, (int []) {0, 3, 6});
    score += _fill_rod(repr, lhs, CLR_G, (int []) {2, 5, 8}, rhs, CLR_O, (int []) {0, 3, 6});
    score += _fill_rod(repr, lhs, CLR_O, (int []) {2, 5, 8}, rhs, CLR_B, (int []) {0, 3, 6});

    score += _score_col(repr, CLR_R, 1);
    score += _score_col(repr, CLR_G, 1);
    score += _score_col(repr, CLR_O, 1);
    score += _score_col(repr, CLR_B, 1);

    score += _fill_rod(repr, lhs, CLR_R, (int []) {0, 1, 2}, rhs, CLR_Y, (int []) {6, 7, 8});
    score += _fill_rod(repr, lhs, CLR_R, (int []) {6, 7, 8}, rhs, CLR_W, (int []) {0, 1, 2});
    score += _fill_rod(repr, lhs, CLR_O, (int []) {0, 1, 2}, rhs, CLR_Y, (int []) {2, 1, 0});
    score += _fill_rod(repr, lhs, CLR_O, (int []) {6, 7, 8}, rhs, CLR_W, (int []) {8, 7, 6});
    
    score += _score_row(Repr_get_rc((Repr *) repr, CLR_R, 1, 0));
    score += _score_row(Repr_get_rc((Repr *) repr, CLR_Y, 1, 0));
    score += _score_row(Repr_get_rc((Repr *) repr, CLR_O, 1, 0));
    score += _score_row(Repr_get_rc((Repr *) repr, CLR_W, 1, 0));

    score += _fill_rod(repr, lhs, CLR_G, (int []) {0, 1, 2}, rhs, CLR_Y, (int []) {8, 5, 2});
    score += _fill_rod(repr, lhs, CLR_G, (int []) {6, 7, 8}, rhs, CLR_W, (int []) {2, 5, 8});
    score += _fill_rod(repr, lhs, CLR_B, (int []) {0, 1, 2}, rhs, CLR_Y, (int []) {0, 3, 6});
    score += _fill_rod(repr, lhs, CLR_B, (int []) {6, 7, 8}, rhs, CLR_W, (int []) {6, 3, 0});

    score += _score_row(Repr_get_rc((Repr *) repr, CLR_B, 1, 0));
    score += _score_col(repr, CLR_Y, 1);
    score += _score_row(Repr_get_rc((Repr *) repr, CLR_G, 1, 0));
    score += _score_col(repr, CLR_W, 1);

    return score;
}

int Repr_score_test(Repr const * repr)
{
    return (Repr_score_rod(repr) + 1) * (Repr_score(repr) + 1);
}

#define TRIPLE_SCORE    -4
#define DOUBLE_SCORE    -2
#define NO_SCORE        0

static int _score_central_bar(Repr const * repr, CLR clr, int idxs[3])
{
    if (Repr_getv(repr, clr, idxs[0]) == Repr_getv(repr, clr, idxs[1]) && 
        Repr_getv(repr, clr, idxs[1]) == Repr_getv(repr, clr, idxs[2]))
    {
        return TRIPLE_SCORE;
    }
    if (Repr_getv(repr, clr, idxs[0]) == Repr_getv(repr, clr, idxs[1]) || 
        Repr_getv(repr, clr, idxs[1]) == Repr_getv(repr, clr, idxs[2]))
    {
        return DOUBLE_SCORE;
    }

    return NO_SCORE;
}

static int _score_side_bar(Repr const * repr, CLR face, int fidxs[3], CLR side, int sidxs[3])
{
    if (Repr_getv(repr, face, fidxs[0]) == Repr_getv(repr, face, fidxs[1]) && 
        Repr_getv(repr, face, fidxs[1]) == Repr_getv(repr, face, fidxs[2]) &&
        Repr_getv(repr, side, sidxs[0]) == Repr_getv(repr, side, sidxs[1]) && 
        Repr_getv(repr, side, sidxs[1]) == Repr_getv(repr, side, sidxs[2]))
    {
        return TRIPLE_SCORE;
    }
    if (Repr_getv(repr, face, fidxs[0]) == Repr_getv(repr, face, fidxs[1]) &&
        Repr_getv(repr, side, sidxs[0]) == Repr_getv(repr, side, sidxs[1]))
    {
        return DOUBLE_SCORE;
    }
    if (Repr_getv(repr, face, fidxs[1]) == Repr_getv(repr, face, fidxs[2]) &&
        Repr_getv(repr, side, sidxs[1]) == Repr_getv(repr, side, sidxs[2]))
    {
        return DOUBLE_SCORE;
    }

    return NO_SCORE;
}

int Repr_score_bars(Repr const * repr)
{
    int score;

    score = 0;
    score += _score_side_bar(repr, CLR_R, (int[]) {0, 1, 2}, CLR_Y, (int []) {6, 7, 8});
    score += _score_side_bar(repr, CLR_R, (int []) {6, 7, 8}, CLR_W, (int []) {0, 1, 2});
    score += _score_side_bar(repr, CLR_O, (int []) {0, 1, 2}, CLR_Y, (int []) {2, 1, 0});
    score += _score_side_bar(repr, CLR_O, (int []) {6, 7, 8}, CLR_W, (int []) {8, 7, 6});
    score += _score_central_bar(repr, CLR_Y, (int []) {3, 4, 5});
    score += _score_central_bar(repr, CLR_W, (int []) {3, 4, 5});
    score += _score_central_bar(repr, CLR_O, (int []) {3, 4, 5});
    score += _score_central_bar(repr, CLR_R, (int []) {3, 4, 5});

    score += _score_side_bar(repr, CLR_G, (int []) {0, 3, 6}, CLR_R, (int []) {2, 5, 8});
    score += _score_side_bar(repr, CLR_G, (int []) {2, 5, 8}, CLR_O, (int []) {0, 3, 6});
    score += _score_side_bar(repr, CLR_B, (int []) {0, 3, 6}, CLR_O, (int []) {2, 5, 8});
    score += _score_side_bar(repr, CLR_B, (int []) {2, 5, 8}, CLR_R, (int []) {0, 3, 6});
    score += _score_central_bar(repr, CLR_R, (int []) {1, 4, 7});
    score += _score_central_bar(repr, CLR_G, (int []) {1, 4, 7});
    score += _score_central_bar(repr, CLR_O, (int []) {1, 4, 7});
    score += _score_central_bar(repr, CLR_B, (int []) {1, 4, 7});

    score += _score_side_bar(repr, CLR_G, (int []) {0, 1, 2}, CLR_Y, (int []) {8, 5, 2});
    score += _score_side_bar(repr, CLR_G, (int []) {6, 7, 8}, CLR_W, (int []) {2, 5, 8});
    score += _score_side_bar(repr, CLR_B, (int []) {0, 1, 2}, CLR_Y, (int []) {0, 3, 6});
    score += _score_side_bar(repr, CLR_B, (int []) {6, 7, 8}, CLR_W, (int []) {6, 3, 0});
    score += _score_central_bar(repr, CLR_Y, (int []) {1, 4, 7});
    score += _score_central_bar(repr, CLR_G, (int []) {3, 4, 5});
    score += _score_central_bar(repr, CLR_W, (int []) {1, 4, 7});
    score += _score_central_bar(repr, CLR_B, (int []) {3, 4, 5});

    return score;
}

static int _CLR_distance(CLR lhs, CLR rhs)
{
    static const int _dist[CLR_$][CLR_$] =
    {
        [CLR_R] = {0, 1, 2, 1, 1, 1},
        [CLR_G] = {1, 0, 1, 2, 1, 1},
        [CLR_O] = {2, 1, 0, 1, 1, 1},
        [CLR_B] = {1, 2, 1, 0, 1, 1},
        [CLR_Y] = {1, 1, 1, 1, 0, 2},
        [CLR_W] = {1, 1, 1, 1, 2, 0},
    };

    return _dist[lhs][rhs];
}

int Repr_score_distance(Repr const * repr)
{
    int score;

    score = 0;
    for (CLR clr = CLR_R; clr < CLR_$; clr ++)
    {
        for (int k = 0; k < DIM * DIM; k ++)
        {
            score += _CLR_distance(clr, CLR_fromc(Repr_getv(repr, clr, k)));
        }
    }

    return score;
}

static int _score_edge(Repr const * repr, CLR sides[2], int idxs[2])
{
    return ! (CLR_fromc(Repr_getv(repr, sides[0], idxs[0])) == sides[0] &&
            CLR_fromc(Repr_getv(repr, sides[1], idxs[1])) == sides[1]);
}

static int _score_corner(Repr const * repr, CLR sides[3], int idxs[3])
{
    return ! (CLR_fromc(Repr_getv(repr, sides[0], idxs[0])) == sides[0] &&
            CLR_fromc(Repr_getv(repr, sides[1], idxs[1])) == sides[1] &&
            CLR_fromc(Repr_getv(repr, sides[2], idxs[2])) == sides[2]);
}

int Repr_score_misplaced(Repr const * repr)
{
    int score;

    score = 0;
    score += _score_edge(repr, (CLR []) {CLR_R, CLR_Y}, (int []) {1, 7});
    score += _score_edge(repr, (CLR []) {CLR_R, CLR_G}, (int []) {5, 3});
    score += _score_edge(repr, (CLR []) {CLR_R, CLR_W}, (int []) {7, 1});
    score += _score_edge(repr, (CLR []) {CLR_R, CLR_B}, (int []) {3, 5});
    score += _score_edge(repr, (CLR []) {CLR_G, CLR_Y}, (int []) {1, 5});
    score += _score_edge(repr, (CLR []) {CLR_G, CLR_O}, (int []) {5, 3});
    score += _score_edge(repr, (CLR []) {CLR_G, CLR_W}, (int []) {7, 5});
    score += _score_edge(repr, (CLR []) {CLR_O, CLR_Y}, (int []) {1, 1});
    score += _score_edge(repr, (CLR []) {CLR_O, CLR_B}, (int []) {5, 3});
    score += _score_edge(repr, (CLR []) {CLR_O, CLR_W}, (int []) {7, 7});
    score += _score_edge(repr, (CLR []) {CLR_B, CLR_Y}, (int []) {1, 3});
    score += _score_edge(repr, (CLR []) {CLR_B, CLR_W}, (int []) {7, 3});

    score += _score_corner(repr, (CLR []) {CLR_R, CLR_Y, CLR_B}, (int []) {0, 6, 2});
    score += _score_corner(repr, (CLR []) {CLR_R, CLR_Y, CLR_G}, (int []) {2, 8, 0});
    score += _score_corner(repr, (CLR []) {CLR_R, CLR_W, CLR_G}, (int []) {8, 2, 6});
    score += _score_corner(repr, (CLR []) {CLR_R, CLR_W, CLR_B}, (int []) {6, 0, 8});
    score += _score_corner(repr, (CLR []) {CLR_O, CLR_Y, CLR_G}, (int []) {0, 2, 2});
    score += _score_corner(repr, (CLR []) {CLR_O, CLR_Y, CLR_B}, (int []) {2, 0, 0});
    score += _score_corner(repr, (CLR []) {CLR_O, CLR_W, CLR_B}, (int []) {8, 6, 6});
    score += _score_corner(repr, (CLR []) {CLR_O, CLR_W, CLR_G}, (int []) {6, 8, 8});

    return score;
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
    for (CLR clr = 0; clr < CLR_$; clr ++)
    {
        _dbg_side(repr, clr, "%c");
    }
    $nl;
}
