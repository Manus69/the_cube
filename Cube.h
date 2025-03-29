#ifndef CUBE_H
#define CUBE_H

#include "Repr.h"

typedef struct Cube Cube;

typedef struct
{
    Vector3 center;
    Model   side_models[CLR_$];
    Vector3 side_pos[CLR_$];
    CLR     side_colors[CLR_$];
}   Block;

typedef struct
{
    CLR     side_clr;
    float   w;
    float   phi;
}   Animation;

struct Cube
{
    Block       blocks[DIM * DIM * DIM];
    Repr        idx_repr;
    Animation   anm;
};

Cube *  Cube_new(int size);
void    Cube_init(Cube * cube);
void    Cube_del(Cube * cube);
void    Cube_draw(Cube const * cube);
void    Cube_rot(Cube * cube, CLR side, float w);
void    Cube_update(Cube * cube);
bool    Cube_in_animation(Cube const * cube);

#endif