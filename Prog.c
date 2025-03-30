#include "Prog.h"
#include "Cube.h"
#include "Solver.h"
#include <stdlib.h>

#define WN      "The Cube"
#define WW      800
#define WH      600
#define TFPS    60
#define SPD0    3
#define SPD1    4

typedef enum
{
    CNTRL_SPACE = CLR_$,
    CNTRL_S,
    CNTRL_TAB, 

    // hold down btns

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
    Repr    repr;
    Cube    cube;
    Camera  cam;
    Input   input;
    Deq     cmd_queue;
    Solver  solver;
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
    if (! Solver_new(& prog->solver))               return NULL;

    Repr_init(& prog->repr, CUBE_CLR_STR);
    _graphics_init(prog);
    _Cam_init(prog);
    Cube_init(& prog->cube);

    prog->runs = true;

    return prog;
}

void Prog_del(Prog * prog)
{
    Deq_del(& prog->cmd_queue);
    Solver_del(& prog->solver);
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

static void _get_shuffle(Prog * prog, int len, char speed)
{
    Cmd cmd;
    Cmd prev = {};
    u64 x;

    x = * (u64 *) (& (double) { GetFrameTime() });

    for (int k = 0; k < len; k ++)
    {
        cmd.clr = rng_xor(& x) % CLR_$;
        cmd.dir = (char []){speed, -speed}[rng_xor(& x) % 2];

        if (prev.clr == cmd.clr && prev.dir == -cmd.dir) cmd.dir *= -1;

        Deq_pushr_check(& prog->cmd_queue, & cmd);
        prev = cmd;
    }
}

static void _queue_speed_adjust(Prog * prog, int val)
{
    Cmd * cmd;

    for (int k = 0; k < Deq_len(& prog->cmd_queue); k ++)
    {
        cmd = Deq_get(& prog->cmd_queue, k);
        cmd->dir *= val;
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
                            [CNTRL_TAB] = KEY_TAB,
                            [CNTRL_S] = KEY_S,
    };

    Cmd cmd = {};

    for (int k = 0; k <= CNTRL_TAB; k ++)
    {
        prog->input.inputs[k] = IsKeyPressed(_key_map[k]);
    }

    for (int k = CNTRL_TAB; k < CNTRL_$; k ++)
    {
        prog->input.inputs[k] = IsKeyDown(_key_map[k]);
    }

    if (WindowShouldClose())                { prog->runs = false; return; }
    if (prog->input.inputs[CNTRL_LEFT])     _Cam_roty(& prog->cam, CAM_W);
    if (prog->input.inputs[CNTRL_RIGHT])    _Cam_roty(& prog->cam, -CAM_W);
    
    if (Deq_len(& prog->cmd_queue) > 1)     { return ; }
    for (int k = CLR_R; k < CLR_$; k ++)
    {
        if      (prog->input.inputs[CNTRL_SHIFT] && prog->input.inputs[k]) cmd = (Cmd) {k, -SPD0};
        else if (prog->input.inputs[k]) cmd = (Cmd) {k, SPD0};
    }
    
    if (cmd.dir) { Deq_pushr_check(& prog->cmd_queue, & cmd); return; }
    if (prog->input.inputs[CNTRL_SPACE])    _get_shuffle(prog, 20, SPD1);

}

void Prog_update(Prog * prog)
{
    Cmd cmd;

    if (! Deq_empty(& prog->cmd_queue) && ! Cube_in_animation(& prog->cube))
    {
        cmd = $drf(Cmd) Deq_popl(& prog->cmd_queue);
        
        Cube_rot(& prog->cube, cmd.clr, cmd.dir);
        Repr_rot(& prog->repr, cmd.clr, cmd.dir);

        //
        // printf("CMD :      (%d %d)\n", cmd.clr, cmd.dir);
        // printf("cum score:  %d\n", Repr_score_cum(& prog->repr));
        // printf("rod score : %d\n", Repr_score_rod(& prog->repr));
    }
    else if (prog->input.inputs[CNTRL_TAB] && ! Cube_in_animation(& prog->cube))
    {
        Cube_reset_clr(& prog->cube);
        Repr_init(& prog->repr, CUBE_CLR_STR);
    }
    else if (prog->input.inputs[CNTRL_S] && ! Cube_in_animation(& prog->cube) && Deq_empty(& prog->cmd_queue))
    {
        Solver_solve(& prog->solver, & prog->repr, & prog->cmd_queue);
        _queue_speed_adjust(prog, 4);
        //
        printf("%d\n", Deq_len(& prog->cmd_queue));
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