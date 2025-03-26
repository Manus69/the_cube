#define _SLIB_IMPL
#include "../_slib/_slib.h"
#include <raylib.h>
#include <raymath.h>

static Camera _cam_get(void)
{
    Camera cam = {};

    cam.position = (Vector3){13, 13, 13};
    cam.target = (Vector3){};
    cam.up = (Vector3){0, 1, 0};
    cam.fovy = 45;
    cam.projection = CAMERA_PERSPECTIVE;

    return cam;
}

void Cam_roty(Camera3D * cam, float x)
{
    cam->position = Vector3Transform(cam->position, MatrixRotateY(x * DEG2RAD));
}

void Cam_rotv(Camera3D * cam, float x)
{
    cam->position = Vector3Transform(cam->position, MatrixRotate((Vector3){1, 0, -1}, x * DEG2RAD));
}

#define HALF_SIZE 1

typedef enum
{
    CLR_R,
    CLR_B,
    CLR_O,
    CLR_G,
    CLR_Y,
    CLR_W,
    CLR_$,
}   CLR;

static CLR CLR_fromc(char x)
{
    static const CLR _tbl[] =
    {
        ['r'] = CLR_R,
        ['b'] = CLR_B,
        ['o'] = CLR_O,
        ['g'] = CLR_G,
        ['y'] = CLR_Y,
        ['w'] = CLR_W,
        ['_'] = CLR_$,
    };

    return _tbl[(int) x];
}

static const Color _colors[] =
{
    RED, BLUE, ORANGE, GREEN, YELLOW, WHITE, DARKGRAY,
};

typedef struct
{
    Vector3 center;
    Model   side_models[CLR_$];
    Vector3 side_pos[CLR_$];
    CLR     side_colors[CLR_$];
}   Block;

void Block_init(Block * block, Vector3 center, const char * clr_str)
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
        block->side_colors[k] = CLR_fromc(clr_str[k]);
        block->side_models[k] = LoadModelFromMesh(mesh);
    }

    f = MatrixRotateY(PI / 4);
    block->side_models[CLR_R].transform = MatrixMultiply(f, MatrixRotateX(PI / 2));
    block->side_models[CLR_B].transform = MatrixMultiply(f, MatrixRotateZ(PI / 2));
    block->side_models[CLR_O].transform = MatrixMultiply(f, MatrixRotateX(-PI / 2));
    block->side_models[CLR_G].transform = MatrixMultiply(f, MatrixRotateZ(-PI / 2));
    block->side_models[CLR_Y].transform = f;
    block->side_models[CLR_W].transform = MatrixMultiply(f, MatrixRotateX(PI));
}

void Block_init_grid(Block * block, int x, int y, int z, const char * clr_str)
{
    Vector3 pos;

    pos = (Vector3){x * HALF_SIZE * 2, y * HALF_SIZE * 2, z * HALF_SIZE * 2};

    Block_init(block, pos, clr_str);
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
        // DrawModelWires(block->side_models[k], block->side_pos[k], 1, BLACK);
    }
}

void draw_Blocks(const Block * blocks, int n)
{
    $mem_map(blocks, Block, n, Block_draw);
}

#define CUBE_SIZE 3
typedef struct
{
    Block blocks[CUBE_SIZE * CUBE_SIZE * CUBE_SIZE];
}   Cube;

void Cube_init(Cube * cube, const char * repr)
{
    
}

int main()
{
    Camera cam;
    InitWindow(800, 600, 0);
    SetTargetFPS(60);

    cam = _cam_get();

    // Block block;
    Block blocks[2];

    // Block_init(& blocks[0], (Vector3){}, "rbog_w");
    Block_init_grid(& blocks[0], 0, 0, 1, "r_____");
    Block_init_grid(& blocks[1], 1, 1, 0, "rb_g__");

    while (! WindowShouldClose())
    {
        if (IsKeyDown(KEY_LEFT))
        {
            Cam_roty(& cam, 2);
        }
        if (IsKeyDown(KEY_RIGHT))
        {
            Cam_roty(& cam, -2);
        }
        if (IsKeyDown(KEY_UP))
        {
            Cam_rotv(& cam, 2);
        }
        if (IsKeyDown(KEY_DOWN))
        {
            Cam_rotv(& cam, -2);
        }
        if (IsKeyDown(KEY_SPACE))
        {
            Block_rot(& blocks[0], (Vector3){1, 0, 0}, 2);
            Block_rot(& blocks[1], (Vector3){1, 0, 0}, 2);

        }


       
        BeginDrawing();
        ClearBackground(LIGHTGRAY);

        BeginMode3D(cam);
        DrawGrid(100, 1);

        // Block_draw(& block);
        draw_Blocks(blocks, 2);

        EndMode3D();

        DrawFPS(10, 10);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}