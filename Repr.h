#include "../_slib/_slib.h"
#include "master.h"

typedef struct
{
    byte    buff[1 << 8];
    int     side_len;
}   Repr;

byte *  Repr_side(Repr * repr, CLR clr);
void    Repr_rot(Repr * repr, CLR clr, int dir);




