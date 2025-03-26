#define _SLIB_IMPL
#include "../_slib/_slib.h"
#include <raylib.h>
#include <raymath.h>

static Camera _cam_get(void)
{
    Camera cam = {};

    cam.position = (Vector3){6, 6, 6};
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
#define NSIDES 6

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
}   Cube;

void Cube_init(Cube * cube, Vector3 center, const char * clr_str)
{
    Mesh    mesh;

    * cube = (Cube) {};
    mesh = GenMeshPoly(4, (HALF_SIZE * 2) / sqrt(2));

    cube->center = center;

    cube->side_pos[CLR_R] = Vector3Add(center, (Vector3){0, 0, HALF_SIZE});
    cube->side_pos[CLR_B] = Vector3Add(center, (Vector3){-HALF_SIZE, 0, 0});
    cube->side_pos[CLR_O] = Vector3Add(center, (Vector3){0, 0, -HALF_SIZE});
    cube->side_pos[CLR_G] = Vector3Add(center, (Vector3){HALF_SIZE, 0, 0});
    cube->side_pos[CLR_Y] = Vector3Add(center, (Vector3){0, HALF_SIZE, 0});
    cube->side_pos[CLR_W] = Vector3Add(center, (Vector3){0, -HALF_SIZE, 0});

    for (int k = 0; k < CLR_$; k ++)
    {
        cube->side_colors[k] = CLR_fromc(clr_str[k]);
        cube->side_models[k] = LoadModelFromMesh(mesh);
    }

    cube->side_models[CLR_R].transform = MatrixMultiply(MatrixRotateY(PI / 4), MatrixRotateX(PI / 2));
    cube->side_models[CLR_B].transform = MatrixMultiply(MatrixRotateY(PI / 4), MatrixRotateZ(PI / 2));
    cube->side_models[CLR_O].transform = MatrixMultiply(MatrixRotateY(PI / 4), MatrixRotateX(-PI / 2));
    cube->side_models[CLR_G].transform = MatrixMultiply(MatrixRotateY(PI / 4), MatrixRotateZ(-PI / 2));
    cube->side_models[CLR_Y].transform = MatrixRotateY(PI / 4);
    cube->side_models[CLR_W].transform = MatrixMultiply(MatrixRotateY(PI / 4), MatrixRotateX(PI));
}

void Cube_init_grid(Cube * cube, int x, int y, int z, const char * clr_str)
{
    Vector3 pos;

    pos = (Vector3){x * HALF_SIZE * 2, y * HALF_SIZE * 2, z * HALF_SIZE * 2};

    Cube_init(cube, pos, clr_str);
}

void Cube_rot(Cube * cube, Vector3 axis, float x)
{
    Matrix f;

    x = x * DEG2RAD;
    f = MatrixRotate(axis, x);
    cube->center = Vector3Transform(cube->center, f);

    for (int k = 0; k < CLR_$; k ++)
    {
        cube->side_models[k].transform = MatrixMultiply(cube->side_models[k].transform, f);
        cube->side_pos[k] = Vector3Transform(cube->side_pos[k], f);
    }
}

void Cube_draw(const Cube * cube)
{
    for (int k = 0; k < CLR_$; k ++)
    {
        DrawModel(cube->side_models[k], cube->side_pos[k], 1, _colors[cube->side_colors[k]]);
        // DrawModelWires(cube->side_models[k], cube->side_pos[k], 1, BLACK);
    }
}

void draw_Cubes(const Cube * cube, int n)
{
    mem_map(cube, n, sizeof(Cube), Cube_draw);
}



int main()
{
    Camera cam;
    InitWindow(800, 600, 0);
    SetTargetFPS(60);

    cam = _cam_get();

    // Cube cube;
    Cube cubes[2];

    // Cube_init(& cubes[0], (Vector3){}, "rbog_w");
    Cube_init_grid(& cubes[0], 0, 0, 1, "r_____");
    Cube_init_grid(& cubes[1], 1, 1, 0, "rb_g__");

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
            Cube_rot(& cubes[0], (Vector3){1, 0, 0}, 2);
        }


       
        BeginDrawing();
        ClearBackground(LIGHTGRAY);

        BeginMode3D(cam);
        DrawGrid(100, 1);

        // Cube_draw(& cube);
        draw_Cubes(cubes, 2);

        EndMode3D();

        DrawFPS(10, 10);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}