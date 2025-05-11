#include "Cube.h"
#include <stdlib.h>

#define DELTA           0.05
#define HALF_SIZE       1

static const Color _colors[] =
{
    RED, GREEN, ORANGE, BLUE, YELLOW, WHITE, DARKGRAY,
};

static void _Block_gen_models(Block * block)
{
    Mesh    mesh;

    mesh = GenMeshPoly(4, (HALF_SIZE * 2) / sqrt(2));

    for (int k = 0; k < CLR_$; k ++)
    {
        block->side_models[k] = LoadModelFromMesh(mesh);
        block->side_colors[k] = CLR_$;
    }
}

static void _Block_init_pos(Block * block, Vector3 center)
{
    Matrix  f;

    block->center = center;

    block->side_pos[CLR_R] = Vector3Add(center, (Vector3){0, 0, HALF_SIZE});
    block->side_pos[CLR_B] = Vector3Add(center, (Vector3){-HALF_SIZE, 0, 0});
    block->side_pos[CLR_O] = Vector3Add(center, (Vector3){0, 0, -HALF_SIZE});
    block->side_pos[CLR_G] = Vector3Add(center, (Vector3){HALF_SIZE, 0, 0});
    block->side_pos[CLR_Y] = Vector3Add(center, (Vector3){0, HALF_SIZE, 0});
    block->side_pos[CLR_W] = Vector3Add(center, (Vector3){0, -HALF_SIZE, 0});

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

    _Block_init_pos(block, pos);
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
    for (int k = 0; k < n; k ++)
    {
        Block_draw(blocks + k);
    }
}

void Cube_draw(Cube const * cube)
{
    draw_Blocks(cube->blocks, DIM * DIM * DIM);
}

static bool _Animation_active(Animation const * anm)
{
    return anm->k < anm->n;
}

static bool _Animation_done(Animation const * anm)
{
    return anm->k == anm->n;
}

static void _Animation_init(Animation * anm, CLR clr, char velocity)
{
    int     n;
    float   dx;

    n = 30 / abs(velocity);
    dx = (90.0 / n) * ((float []){1, -1}[velocity < 0]);

    * anm = (Animation) { .dx = dx, .n = n, .side_clr = clr };
}

static void _Animation_reset(Animation * anm)
{
    anm->n = 0;
}

static bool _Animation_inc(Animation * anm)
{
    anm->k ++;

    return ! _Animation_done(anm);
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

static bool _rot(Cube * cube, CLR clr_side, float dx)
{
    byte *  side;
    int     idx;
    Vector3 axis;

    side = Repr_side(& cube->idx_repr, clr_side);
    axis = _CLR_axis_map(clr_side);

    for (int k = 0; k < DIM * DIM; k ++)
    {
        idx = side[k];
        Block_rot(cube->blocks + idx, axis, dx);
    }

    return ! _Animation_inc(& cube->anm);
}

bool Cube_in_animation(Cube const * cube)
{
    return _Animation_active(& cube->anm);
}

void Cube_update(Cube * cube)
{
    if (! _Animation_active(& cube->anm)) return ;

    if (_rot(cube, cube->anm.side_clr, cube->anm.dx))
    {
        Repr_rot(& cube->idx_repr, cube->anm.side_clr, cube->anm.dx > 0 ? 1 : -1);
        
        _Animation_reset(& cube->anm);
    }
}

void Cube_rot(Cube * cube, CLR clr_side, char velocity)
{
    if (_Animation_active(& cube->anm)) return ;

    _Animation_init(& cube->anm, clr_side, velocity);
}

static int _repr_idx_map(CLR side, int row, int col)
{
    static const int vals[CLR_$][DIM * DIM] =
    {
        {8, 17, 26, 5, 14, 23, 2, 11, 20},
        {26, 25, 24, 23, 22, 21, 20, 19, 18},
        {24, 15, 6, 21, 12, 3, 18, 9, 0},
        {6, 7, 8, 3, 4, 5, 0, 1, 2},
        {6, 15, 24, 7, 16, 25, 8, 17, 26},
        {2, 11, 20, 1, 10, 19, 0, 9, 18},
    };

    return vals[side][_row_col_idx(row, col)];
}

static void _init_blocks(Cube * cube)
{
    int idx;

    idx = 0;
    for (int x = 0; x < DIM; x ++)
    {
        for (int y = 0; y < DIM; y ++)
        {
            for (int z = 0; z < DIM; z ++)
            {
                Block_init_grid(& cube->blocks[idx], x - DIM / 2, y - DIM / 2, z - DIM / 2);
                idx ++;
            }
        }
    }
}

static void _init_colors(Cube * cube, byte const * clr_repr)
{
    int idx;
    CLR current;

    for (CLR clr = CLR_R; clr < CLR_$; clr ++)
    {
        for (int k = 0; k < DIM * DIM; k ++)
        {
            idx = * Repr_get(& cube->idx_repr, clr, k);
            current = CLR_fromc(clr_repr[clr * DIM * DIM + k]);
            cube->blocks[idx].side_colors[clr] = current;
        }
    }
}

static void _init_side_repr(Cube * cube)
{
    byte *  side;
    int     idx;

    for (CLR clr = CLR_R; clr < CLR_$; clr ++)
    {
        side = Repr_side(& cube->idx_repr, clr);
        for (int k = 0; k < DIM * DIM; k ++)
        {
            idx = _repr_idx_map(clr, _idx_row(k), _idx_col(k));
            side[k] = (byte) idx;
        }
    }
}

void Cube_reset_clr(Cube * cube)
{
    _init_blocks(cube);
    _init_side_repr(cube);
    _init_colors(cube, (byte *) CUBE_CLR_STR);
}

void Cube_init(Cube * cube)
{
    for (int k = 0; k < DIM * DIM * DIM; k ++)
    {
        _Block_gen_models(cube->blocks + k);
    }

    Cube_reset_clr(cube);
}

void Cube_del(Cube * cube)
{
    free(cube);
}