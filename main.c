#define _SLIB_IMPL
#include "../_slib/_slib.h"
#include "Prog.h"

// blocks get misaligned at higher fps

int main()
{
    Prog * prog;

    if (! (prog = Prog_new())) abort();
    
    while (true)
    {
        Prog_draw(prog);
        Prog_input(prog);

        if (! Prog_runs(prog)) break;

        Prog_update(prog);
    }

    Prog_del(prog);
    return 0;
}

// #define CAM_ROT_W   2
// #define CUBE_ROT_W  180

// static Camera _cam_get(void)
// {
//     Camera cam = {};

//     cam.position = (Vector3){13, 13, 13};
//     cam.target = (Vector3){};
//     cam.up = (Vector3){0, 1, 0};
//     cam.fovy = 45;
//     cam.projection = CAMERA_PERSPECTIVE;

//     return cam;
// }

// void Cam_roty(Camera3D * cam, float x)
// {
//     cam->position = Vector3Transform(cam->position, MatrixRotateY(x * DEG2RAD));
// }

// void Cam_rotv(Camera3D * cam, float x)
// {
//     cam->position = Vector3Transform(cam->position, MatrixRotate((Vector3){1, 0, -1}, x * DEG2RAD));
// }

// typedef struct
// {
//     CLR     clr;
//     float   w;
// }   Action;

// static void _try_action(Cube * cube)
// {
//     static Action actions[] =
//     {
//         {CLR_R, CUBE_ROT_W},
//         {CLR_R, CUBE_ROT_W},
//         {CLR_O, CUBE_ROT_W},
//         {CLR_O, CUBE_ROT_W},
//         {CLR_Y, CUBE_ROT_W},
//         {CLR_Y, CUBE_ROT_W},
//         {CLR_W, CUBE_ROT_W},
//         {CLR_W, CUBE_ROT_W},
//         {CLR_G, CUBE_ROT_W},
//         {CLR_G, CUBE_ROT_W},
//         {CLR_B, CUBE_ROT_W},
//         {CLR_B, CUBE_ROT_W},
//     };

//     static int k;

//     if (Cube_in_animation(cube))    return ;
//     if (k == sizeof(actions) / sizeof(Action)) return ;

//     Cube_rot(cube, actions[k].clr, actions[k].w);
//     k ++;
// }

// //deq is broken
// int main()
// {
//     Camera cam;
//     InitWindow(800, 600, 0);
//     SetTargetFPS(60);

//     cam = _cam_get();
//     Cube * cube = Cube_new(3);

//     while (! WindowShouldClose())
//     {
//         if (IsKeyDown(KEY_LEFT))    Cam_roty(& cam, CAM_ROT_W);
//         if (IsKeyDown(KEY_RIGHT))   Cam_roty(& cam, -CAM_ROT_W);
//         if (IsKeyDown(KEY_UP))      Cam_rotv(& cam, CAM_ROT_W);
//         if (IsKeyDown(KEY_DOWN))    Cam_rotv(& cam, -CAM_ROT_W);

//         if (IsKeyDown(KEY_R) && IsKeyDown(KEY_LEFT_SHIFT)) Cube_rot(cube, CLR_R, -CUBE_ROT_W);
//         else if (IsKeyDown(KEY_R)) Cube_rot(cube, CLR_R, CUBE_ROT_W);
        
//         if (IsKeyDown(KEY_LEFT_SHIFT) && IsKeyDown(KEY_Y)) Cube_rot(cube, CLR_Y, -CUBE_ROT_W);
//         else if (IsKeyDown(KEY_Y)) Cube_rot(cube, CLR_Y, CUBE_ROT_W);

//         if (IsKeyDown(KEY_LEFT_SHIFT) && IsKeyDown(KEY_G)) Cube_rot(cube, CLR_G, -CUBE_ROT_W);
//         else if (IsKeyDown(KEY_G)) Cube_rot(cube, CLR_G, CUBE_ROT_W);

//         if (IsKeyDown(KEY_LEFT_SHIFT) && IsKeyDown(KEY_B)) Cube_rot(cube, CLR_B, -CUBE_ROT_W);
//         else if (IsKeyDown(KEY_B)) Cube_rot(cube, CLR_B, CUBE_ROT_W);

//         if (IsKeyDown(KEY_LEFT_SHIFT) && IsKeyDown(KEY_O)) Cube_rot(cube, CLR_O, -CUBE_ROT_W);
//         else if (IsKeyDown(KEY_O)) Cube_rot(cube, CLR_O, CUBE_ROT_W);

//         if (IsKeyDown(KEY_LEFT_SHIFT) && IsKeyDown(KEY_W)) Cube_rot(cube, CLR_W, -CUBE_ROT_W);
//         else if (IsKeyDown(KEY_W)) Cube_rot(cube, CLR_W, CUBE_ROT_W);

//         if (IsKeyDown (KEY_SPACE))
//         {
//             _try_action(cube);
//         }

//         Cube_update(cube);

//         BeginDrawing();
//         ClearBackground(LIGHTGRAY);

//         BeginMode3D(cam);
//         DrawGrid(100, 1);

//         Cube_draw(cube);

//         EndMode3D();

//         DrawFPS(10, 10);
//         EndDrawing();
//     }

//     CloseWindow();
//     Cube_del(cube);

//     return 0;
// }