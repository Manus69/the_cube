#include "Prog.h"
#include "Cube.h"
#include <stdlib.h>

#define WN      "The Cube"
#define WW      800
#define WH      600
#define TFPS    120

typedef enum
{
    CNTRL_SPACE = CLR_$,
    CNTR_TAB,
    CNTRL_SHIFT,
    CNTRL_LEFT,
    CNTRL_RIGHT,
    CNTRL_UP,
    CNTRL_DOWN,
    CNTRL_$,
}   CNTRL;

typedef struct
{
    bool    inputs[CNTRL_$];
}   Input;

struct Prog
{
    Cube    cube;
    Camera  cam;
    Input   input;
    Deq     cmd_queue;
    bool    runs;
};

static void _graphics_init(Prog * prog)
{
    (void) prog;

    InitWindow(WW, WH, WN);
    SetTargetFPS(TFPS);
}

static void _Cam_init(Prog * prog)
{
    prog->cam.position =    (Vector3){13, 13, 13};
    prog->cam.target =      (Vector3){};
    prog->cam.up =          (Vector3){0, 1, 0};
    prog->cam.fovy =        45;
    prog->cam.projection =  CAMERA_PERSPECTIVE;
}

#include <stdio.h>
Prog * Prog_new(void)
{
    Prog * prog;

    if (! (prog = calloc(1, sizeof(* prog))))       return NULL;
    if (! Deq_new(& prog->cmd_queue, sizeof(Cmd)))  return NULL;

    _graphics_init(prog);
    _Cam_init(prog);
    Cube_init(& prog->cube);

    prog->runs = true;

    // printf("%zu\n", sizeof(Prog));
    return prog;
}

void Prog_del(Prog * prog)
{
    Deq_del(& prog->cmd_queue);
    CloseWindow();
    free(prog);
}

bool Prog_runs(Prog const * prog)
{
    return prog->runs;
}

static void _Cam_roty(Camera3D * cam, float x)
{
    cam->position = Vector3Transform(cam->position, MatrixRotateY(x * DEG2RAD));
}

// static void _Cam_rotv(Camera3D * cam, float x)
// {
//     cam->position = Vector3Transform(cam->position, MatrixRotate((Vector3){1, 0, -1}, x * DEG2RAD));
// }

static void _test(Prog * prog)
{
    Cmd seq[] = {{0, -1}, {2, 1}, {4, -1}, {5, 1}, {0, 1}, {2, -1}};

    for (int k = 0; (unsigned) k < sizeof(seq) / sizeof(* seq); k ++)
    {
        Deq_pushr_check(& prog->cmd_queue, seq + k);
    }
}

static void _get_shuffle(Prog * prog, int len)
{
    Cmd cmd;
    Cmd prev = {};
    u64 x;

    x = * (u64 *) (& (double) { GetFrameTime() });

    for (int k = 0; k < len; k ++)
    {
        cmd.clr = rng_xor(& x) % CLR_$;
        cmd.dir = (char []){1, -1}[rng_xor(& x) % 2];

        if (prev.clr == cmd.clr && prev.dir == -cmd.dir) cmd.dir *= -1;

        Deq_pushr_check(& prog->cmd_queue, & cmd);
        prev = cmd;
    }
}

#define CAM_W   3
#define CUBE_W  360

void Prog_input(Prog * prog)
{
    static const KeyboardKey _key_map[CNTRL_$] =
    {
        [CLR_R] = KEY_R,    [CNTRL_UP] = KEY_UP,
        [CLR_G] = KEY_G,    [CNTRL_DOWN] = KEY_DOWN,
        [CLR_O] = KEY_O,    [CNTRL_LEFT] = KEY_LEFT,
        [CLR_B] = KEY_B,    [CNTRL_RIGHT] = KEY_RIGHT,
        [CLR_Y] = KEY_Y,    [CNTRL_SHIFT] = KEY_LEFT_SHIFT,
        [CLR_W] = KEY_W,    [CNTRL_SPACE] = KEY_SPACE,
                            [CNTR_TAB] = KEY_TAB,
    };

    Cmd cmd = {};

    for (int k = 0; k <= CNTR_TAB; k ++)
    {
        prog->input.inputs[k] = IsKeyPressed(_key_map[k]);
    }

    for (int k = CNTR_TAB; k < CNTRL_$; k ++)
    {
        prog->input.inputs[k] = IsKeyDown(_key_map[k]);
    }

    if (WindowShouldClose())                { prog->runs = false; return; }
    if (prog->input.inputs[CNTRL_LEFT])     _Cam_roty(& prog->cam, CAM_W);
    if (prog->input.inputs[CNTRL_RIGHT])    _Cam_roty(& prog->cam, -CAM_W);
    
    if (Deq_len(& prog->cmd_queue) > 1)     { return ; }
    for (int k = CLR_R; k < CLR_$; k ++)
    {
        if      (prog->input.inputs[CNTRL_SHIFT] && prog->input.inputs[k]) cmd = (Cmd) {k, -1};
        else if (prog->input.inputs[k]) cmd = (Cmd) {k, 1};
    }
    
    if (cmd.dir) { Deq_pushr_check(& prog->cmd_queue, & cmd); return; }
    // if (prog->input.inputs[CNTRL_SPACE])    _test(prog);
    if (prog->input.inputs[CNTRL_SPACE])    _get_shuffle(prog, 20);

}

void Prog_update(Prog * prog)
{
    Cmd cmd;

    if (! Deq_empty(& prog->cmd_queue) && ! Cube_in_animation(& prog->cube))
    {
        cmd = $drf(Cmd) Deq_popl(& prog->cmd_queue);
        Cube_rot(& prog->cube, cmd.clr, cmd.dir);    
    }
    else if (prog->input.inputs[CNTR_TAB] && ! Cube_in_animation(& prog->cube))
    {
        Cube_reset_clr(& prog->cube);
    }

    Cube_update(& prog->cube);
}

void Prog_draw(Prog * prog)
{
    BeginDrawing();
    ClearBackground(LIGHTGRAY);

    BeginMode3D(prog->cam);
    DrawGrid(100, 1);

    Cube_draw(& prog->cube);

    EndMode3D();

    DrawFPS(10, 10);
    EndDrawing();
}