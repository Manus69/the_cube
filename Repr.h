#ifndef REPR_H
#define REPR_H

#include "master.h"

typedef struct
{
    byte    buff[1 << 6];
}   Repr;

byte *  Repr_side(Repr * repr, CLR clr);
void    Repr_rot(Repr * repr, CLR clr, int dir);
void    Repr_dbg(Repr const * repr, CLR clr);

#endif



