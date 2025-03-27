#ifndef CUBE_H
#define CUBE_H

#include "master.h"

typedef struct Cube Cube;

Cube *  Cube_new(int size);
void    Cube_del(Cube * cube);
void    Cube_draw(Cube const * cube);
void    Cube_rot(Cube * cube, CLR side, float w);
void    Cube_update(Cube * cube);

#endif