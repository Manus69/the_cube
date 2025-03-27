#define _SLIB_IMPL
#include "../_slib/_slib.h"
#include "Cube.h"

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


int main()
{
    Camera cam;
    InitWindow(800, 600, 0);
    SetTargetFPS(60);

    cam = _cam_get();
    Cube * cube = Cube_new(3);

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
            Cube_rot(cube, CLR_R, 2);
        }


        BeginDrawing();
        ClearBackground(LIGHTGRAY);

        BeginMode3D(cam);
        DrawGrid(100, 1);

        Cube_draw(cube);

        EndMode3D();

        DrawFPS(10, 10);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}