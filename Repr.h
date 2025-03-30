#ifndef REPR_H
#define REPR_H

#include "master.h"

#define REPR_LEN (CLR_$ * DIM * DIM)

typedef struct
{
    byte    buff[REPR_LEN];
}   Repr;

int     Repr_score(Repr const * repr);
int     Repr_score_dist(Repr const * repr);
int     Repr_score_rows(Repr const * repr);
int     Repr_score_cum(Repr const * repr);
void    Repr_init(Repr * repr, char const * cstr);
byte *  Repr_get(Repr * repr, CLR clr, int idx);
byte *  Repr_get_rc(Repr * repr, CLR clr, int row, int col);
byte *  Repr_side(Repr * repr, CLR clr);
void    Repr_rot(Repr * repr, CLR clr, int dir);
u64     Repr_hash(Repr const * repr);
u64     Repr_hashf(void const * ptr);
void    Repr_idx_dbg(Repr const * repr, CLR clr);
void    Repr_clr_dbg(Repr const * repr);

#endif



