#include "Repr.h"

static int _score_cmp(Repr const * repr, CLR clr, int row, int col, byte ref)
{
    if (row < 0 || row >= DIM || col < 0 || col >= DIM) return 0;

    return * Repr_get_rc((Repr *) repr, clr, row, col) != ref;
}

static int _score_tile(Repr const * repr, CLR clr, int row, int col)
{
    int     score;
    byte    x;

    x = * Repr_get_rc((Repr *) repr, clr, row, col);
    score = 0;

    score += _score_cmp(repr, clr, row, col - 1, x);
    score += _score_cmp(repr, clr, row, col + 1, x);
    score += _score_cmp(repr, clr, row - 1, col, x);
    score += _score_cmp(repr, clr, row + 1, col, x);

    return score;
}

static int _score_side(Repr const * repr, CLR clr)
{
    int score;

    score = 0;
    for (int row = 0; row < DIM; row ++)
    {
        for (int col = 0; col < DIM; col ++)
        {
            score += _score_tile(repr, clr, row, col);
        }
    }

    return score;
}

int Repr_score_nbghr(Repr const * repr)
{
    int score;

    score = 0;
    for (CLR clr = 0; clr < CLR_$; clr ++)
    {
        score += _score_side(repr, clr);
    }

    return score;
}

static int _score_cross(Repr const * repr, CLR clr, CLR adj[4], int idx[4])
{
    static int  idxs[] = {1, 3, 5, 7};
    int         score;

    score = 0;
    for (int k = 0; k < 4; k ++)
    {
        score += ! ((Repr_is_clr(repr, clr, idxs[k], clr)) && Repr_is_clr(repr, adj[k], idx[k], adj[k]));
    }
    
    return score;
}

int Repr_score_cross(Repr const * repr)
{
    int score;

    score = 0;

    score += _score_cross(repr, CLR_R, (CLR []) {CLR_Y, CLR_B, CLR_G, CLR_W}, (int []) {7, 5, 3, 1});
    score += _score_cross(repr, CLR_G, (CLR []) {CLR_Y, CLR_R, CLR_O, CLR_W}, (int []) {5, 5, 3, 5});
    score += _score_cross(repr, CLR_O, (CLR []) {CLR_Y, CLR_G, CLR_B, CLR_W}, (int []) {1, 5, 3, 7});
    score += _score_cross(repr, CLR_B, (CLR []) {CLR_Y, CLR_O, CLR_R, CLR_W}, (int []) {3, 5, 3, 3});
    score += _score_cross(repr, CLR_Y, (CLR []) {CLR_O, CLR_B, CLR_G, CLR_R}, (int []) {1, 1, 1, 1});
    score += _score_cross(repr, CLR_W, (CLR []) {CLR_R, CLR_B, CLR_G, CLR_O}, (int []) {7, 7, 7, 7});

    return score;
}