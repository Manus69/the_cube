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

void Cam_rot(Camera3D * cam, float x)
{
    cam->position = Vector3Transform(cam->position, MatrixRotateY(x * DEG2RAD));
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
    Matrix  f;

    * cube = (Cube) {};
    mesh = GenMeshPoly(4, HALF_SIZE * 2);

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

    // f = MatrixRotateY(PI / 4);
    // f = MatrixMultiply(f, MatrixRotateX(PI / 2));

    cube->side_models[CLR_R].transform = MatrixMultiply(MatrixRotateY(PI / 4), MatrixRotateX(PI / 2));
    cube->side_models[CLR_B].transform = MatrixMultiply(MatrixRotateY(PI / 4), MatrixRotateZ(PI / 2));

    // cube->side_models[CLR_B]

}

void Cube_draw(const Cube * cube)
{
    // DrawSphere((Vector3){0, 0, 1}, 1, BLACK);
    // DrawModel(cube->side_models[CLR_R], cube->side_pos[CLR_R], 1, _colors[cube->side_colors[CLR_R]]);
    // DrawModelWires(cube->side_models[CLR_R], cube->side_pos[CLR_R], 1, BLACK);
    for (int k = 0; k < 2; k ++)
    {
        DrawModel(cube->side_models[k], cube->side_pos[k], 1, _colors[cube->side_colors[k]]);
        DrawModelWires(cube->side_models[k], cube->side_pos[k], 1, BLACK);
    }
}



int main()
{
    Camera cam;
    InitWindow(800, 600, 0);
    SetTargetFPS(60);

    cam = _cam_get();

    Cube cube;

    Cube_init(& cube, (Vector3){}, "rbogyw");

    while (! WindowShouldClose())
    {
        if (IsKeyDown(KEY_LEFT))
        {
            Cam_rot(& cam, 2);
        }
        if (IsKeyDown(KEY_RIGHT))
        {
            Cam_rot(& cam, -2);
        }
       
        BeginDrawing();
        ClearBackground(LIGHTGRAY);

        BeginMode3D(cam);
        DrawGrid(100, 1);

        Cube_draw(& cube);

        EndMode3D();

        DrawFPS(10, 10);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}