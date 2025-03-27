#include "Cube.h"
#include "Repr.h"
#include <stdlib.h>

#define DELTA           0.05
#define CUBE_DIM        3
#define HALF_SIZE       1
#define CUBE_CLR_STR    "rrrrrrrrr" "ggggggggg" "ooooooooo" "bbbbbbbbb" "yyyyyyyyy" "wwwwwwwww"

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
    Block       blocks[CUBE_DIM * CUBE_DIM * CUBE_DIM];
    Repr        idx_repr;
    Animation   anm;
};

static const Color _colors[] =
{
    RED, GREEN, ORANGE, BLUE, YELLOW, WHITE, DARKGRAY,
};

static inline CLR CLR_fromc(char x)
{
    static const CLR _tbl[] =
    {
        ['r'] = CLR_R,
        ['g'] = CLR_G,
        ['o'] = CLR_O,
        ['b'] = CLR_B,
        ['y'] = CLR_Y,
        ['w'] = CLR_W,
        ['_'] = CLR_$,
    };

    return _tbl[(int) x];
}

void Block_init(Block * block, Vector3 center)
{
    Mesh    mesh;
    Matrix  f;

    * block = (Block) {};
    mesh = GenMeshPoly(4, (HALF_SIZE * 2) / sqrt(2));

    block->center = center;

    block->side_pos[CLR_R] = Vector3Add(center, (Vector3){0, 0, HALF_SIZE});
    block->side_pos[CLR_B] = Vector3Add(center, (Vector3){-HALF_SIZE, 0, 0});
    block->side_pos[CLR_O] = Vector3Add(center, (Vector3){0, 0, -HALF_SIZE});
    block->side_pos[CLR_G] = Vector3Add(center, (Vector3){HALF_SIZE, 0, 0});
    block->side_pos[CLR_Y] = Vector3Add(center, (Vector3){0, HALF_SIZE, 0});
    block->side_pos[CLR_W] = Vector3Add(center, (Vector3){0, -HALF_SIZE, 0});

    for (int k = 0; k < CLR_$; k ++)
    {
        block->side_models[k] = LoadModelFromMesh(mesh);
        block->side_colors[k] = CLR_$;
    }

    f = MatrixRotateY(PI / 4);
    block->side_models[CLR_R].transform = MatrixMultiply(f, MatrixRotateX(PI / 2));
    block->side_models[CLR_B].transform = MatrixMultiply(f, MatrixRotateZ(PI / 2));
    block->side_models[CLR_O].transform = MatrixMultiply(f, MatrixRotateX(-PI / 2));
    block->side_models[CLR_G].transform = MatrixMultiply(f, MatrixRotateZ(-PI / 2));
    block->side_models[CLR_Y].transform = f;
    block->side_models[CLR_W].transform = MatrixMultiply(f, MatrixRotateX(PI));
}

void Block_init_grid(Block * block, int x, int y, int z)
{
    Vector3 pos;

    pos = (Vector3){x * (HALF_SIZE + DELTA) * 2, y * (HALF_SIZE + DELTA) * 2, z * (HALF_SIZE + DELTA) * 2};

    Block_init(block, pos);
}

void Block_rot(Block * block, Vector3 axis, float x)
{
    Matrix f;

    x = x * DEG2RAD;
    f = MatrixRotate(axis, x);
    block->center = Vector3Transform(block->center, f);

    for (int k = 0; k < CLR_$; k ++)
    {
        block->side_models[k].transform = MatrixMultiply(block->side_models[k].transform, f);
        block->side_pos[k] = Vector3Transform(block->side_pos[k], f);
    }
}

void Block_draw(const Block * block)
{
    for (int k = 0; k < CLR_$; k ++)
    {
        DrawModel(block->side_models[k], block->side_pos[k], 1, _colors[block->side_colors[k]]);
        DrawModelWires(block->side_models[k], block->side_pos[k], 1, BLACK);
    }
}

void draw_Blocks(const Block * blocks, int n)
{
    $mem_map(blocks, Block, n, Block_draw);
}

void Cube_draw(Cube const * cube)
{
    draw_Blocks(cube->blocks, 27);
}

static bool _Animation_active(Animation const * anm)
{
    return anm->w > 0.0f || anm->w < 0.0f;
}

static bool _Animation_done(Animation const * anm)
{
    return fabs(anm->phi) >= 90.0f; 
}

static void _Animation_set(Animation * anm, CLR clr, float w)
{
    anm->side_clr = clr;
    anm->w = w;
    anm->phi = 0;
}

static void _Animation_reset(Animation * anm)
{
    anm->phi = 0;
    anm->w = 0;
}

static void _Animation_inc(Animation * anm, float dx)
{
    anm->phi += dx;
    if (_Animation_done(anm)) _Animation_reset(anm);
}

static Vector3 _CLR_axis_map(CLR clr)
{
    static const Vector3 axes[] =
    {
        [CLR_R] = (Vector3){0, 0, 1},
        [CLR_G] = (Vector3){1, 0, 0},
        [CLR_O] = (Vector3){0, 0, -1},
        [CLR_B] = (Vector3){-1, 0, 0},
        [CLR_Y] = (Vector3){0, 1, 0},
        [CLR_W] = (Vector3){0, -1, 0},
    };

    return axes[clr];
}

static void _rot(Cube * cube, CLR clr_side, float w)
{
    byte *  side;
    int     idx;
    Vector3 axis;
    double  dt;

    side = Repr_side(& cube->idx_repr, clr_side);
    axis = _CLR_axis_map(clr_side);
    dt = GetFrameTime();

    for (int k = 0; k < cube->idx_repr.side_len; k ++)
    {
        idx = side[k];
        Block_rot(cube->blocks + idx, axis, w * dt);
    }

    _Animation_inc(& cube->anm, w * dt);
}

void Cube_update(Cube * cube)
{
    if (! _Animation_active(& cube->anm)) return ;

    _rot(cube, cube->anm.side_clr, cube->anm.w);
}

void Cube_rot(Cube * cube, CLR clr_side, float w)
{
    if (_Animation_active(& cube->anm)) return ;

    _Animation_set(& cube->anm, clr_side, w);
}

//
#include <stdio.h>

static int _repr_idx_map(CLR side, int row, int col)
{
    static const int vals[CLR_$][CUBE_DIM * CUBE_DIM] =
    {
        {8, 17, 26, 5, 14, 23, 2, 11, 20},
        {26, 25, 24, 23, 22, 21, 20, 19, 18},
        {24, 15, 6, 21, 12, 3, 18, 9, 0},
        {6, 7, 8, 3, 4, 5, 0, 1, 2},
        {6, 15, 24, 7, 16, 25, 8, 17, 26},
        {2, 11, 20, 1, 10, 19, 0, 9, 18},
    };

    return vals[side][_row_col_idx(row, col, CUBE_DIM)];
}

static void _init_blocks(Cube * cube)
{
    int idx;

    idx = 0;
    for (int x = 0; x < CUBE_DIM; x ++)
    {
        for (int y = 0; y < CUBE_DIM; y ++)
        {
            for (int z = 0; z < CUBE_DIM; z ++)
            {
                // if (y == 0)
                //     printf("id : %d at (%d %d %d)\n", idx, x, y, z);

                Block_init_grid(& cube->blocks[idx], x - CUBE_DIM / 2, y - CUBE_DIM / 2, z - CUBE_DIM / 2);
                idx ++;
            }
        }
    }
}

static void _init_colors(Cube * cube, byte const * clr_repr)
{
    int idx, row, col, r_idx;

    r_idx = 0;
    for (CLR clr = CLR_R; clr < CLR_$; clr ++)
    {
        for (int k = 0; k < CUBE_DIM * CUBE_DIM; k ++)
        {
            row = _idx_row(k, CUBE_DIM);
            col = _idx_col(k, CUBE_DIM);
            idx = _repr_idx_map(clr, row, col);

            // printf("idx : %d clr : %d repr : %u\n", idx, clr, clr_repr[r_idx]);

            cube->blocks[idx].side_colors[clr] = CLR_fromc(clr_repr[r_idx]);
            r_idx ++;
        }
    }
}

static void _init_side_repr(Cube * cube)
{
    byte *  side;
    int     idx;

    cube->idx_repr.side_len = CUBE_DIM * CUBE_DIM;

    for (CLR clr = CLR_R; clr < CLR_$; clr ++)
    {
        side = Repr_side(& cube->idx_repr, clr);
        for (int k = 0; k < cube->idx_repr.side_len; k ++)
        {
            idx = _repr_idx_map(clr, _idx_row(k, CUBE_DIM), _idx_col(k, CUBE_DIM));
            side[k] = (byte) idx;
        }
    }
}

Cube * Cube_new(int size)
{
    (void) size;

    Cube * cube;

    if (! (cube = calloc(1, sizeof(Cube)))) return NULL;
    _init_blocks(cube);
    _init_colors(cube, (byte *) CUBE_CLR_STR);
    _init_side_repr(cube);

    return cube;
}

void Cube_del(Cube * cube)
{
    free(cube);
}