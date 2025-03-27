#define _SLIB_IMPL
#include "../_slib/_slib.h"
#include "Cube.h"

#define CAM_ROT_W   2
#define CUBE_ROT_W  180

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
        if (IsKeyDown(KEY_LEFT))    Cam_roty(& cam, CAM_ROT_W);
        if (IsKeyDown(KEY_RIGHT))   Cam_roty(& cam, -CAM_ROT_W);
        if (IsKeyDown(KEY_UP))      Cam_rotv(& cam, CAM_ROT_W);
        if (IsKeyDown(KEY_DOWN))    Cam_rotv(& cam, -CAM_ROT_W);

        if (IsKeyDown(KEY_R) && IsKeyDown(KEY_LEFT_SHIFT)) Cube_rot(cube, CLR_R, -CUBE_ROT_W);
        else if (IsKeyDown(KEY_R)) Cube_rot(cube, CLR_R, CUBE_ROT_W);
        
        if (IsKeyDown(KEY_LEFT_SHIFT) && IsKeyDown(KEY_Y)) Cube_rot(cube, CLR_Y, -CUBE_ROT_W);
        else if (IsKeyDown(KEY_Y)) Cube_rot(cube, CLR_Y, CUBE_ROT_W);

        Cube_update(cube);

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
    Cube_del(cube);

    return 0;
}