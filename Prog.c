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
    BTN_SPACE = CLR_$,
    BTN_S,
    BTN_TAB,
    BTN_T,
    BTN_SHFT,
    // hold down btns

    BTN_SHIFT, 
    BTN_LEFT,
    BTN_RIGHT,
    BTN_UP,
    BTN_DOWN,
    BTN_$,
}   BTN;

typedef struct
{
    bool    inputs[BTN_$];
}   Input;

typedef struct
{
    Deq     deq;
    byte    vel;
    bool    lock;
}   Cmdq;

static bool Cmdq_init(Cmdq * cmdq)
{
    cmdq->lock = false;
    cmdq->vel = SPD0;

    return Deq_new(& cmdq->deq, sizeof(Cmd));
}

static void Cmdq_lock(Cmdq * cmdq)
{
    cmdq->lock = true;
}

static void Cmdq_unlock(Cmdq * cmdq)
{
    cmdq->lock = false;
}

static void Cmdq_lock_toggle(Cmdq * cmdq)
{
    cmdq->lock = ! cmdq->lock;
}

static bool Cmdq_empty(Cmdq const * cmdq)
{
    return Deq_empty(& cmdq->deq);
}

static bool Cmdq_poppable(Cmdq const * cmdq)
{
    return ! cmdq->lock && ! Cmdq_empty(cmdq);
}

static void Cmdq_vel_inc(Cmdq * cmdq, int dv)
{
    int v;

    v = cmdq->vel + dv;
    if (v < 1) return ;

    cmdq->vel = v;
}

static bool Cmdq_push(Cmdq * cmdq, Cmd const * cmd)
{
    return Deq_pushr(& cmdq->deq, cmd);
}

static void Cmdq_reset(Cmdq * cmdq)
{
    Deq_pop_all(& cmdq->deq);
    Cmdq_unlock(cmdq);
}

struct Prog
{
    Repr    repr;
    Cube    cube;
    Camera  cam;
    Input   input;
    Cmdq    cmdq;
    Solver  solver;
    bool    runs;
};

static void _graphics_init(Prog * prog)
{
    (void) prog;

    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
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
    if (! Cmdq_init(& prog->cmdq))                  return NULL;
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
    Deq_del(& prog->cmdq.deq);
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

static void _get_shuffle(Prog * prog, int len)
{
    Cmd cmd;
    Cmd prev = {};
    u64 x;

    x = * (u64 *) (& (double) { GetFrameTime() });

    for (int k = 0; k < len; k ++)
    {
        cmd.clr = sfl_rng(& x) % CLR_$;
        cmd.dir = (char []){1, -1}[sfl_rng(& x) % 2];

        if (prev.clr == cmd.clr && prev.dir == -cmd.dir) cmd.dir *= -1;

        Cmdq_push(& prog->cmdq, & cmd);
        prev = cmd;
    }
}

static void _test(Prog * prog)
{
    // char const * cmd_cstr = "GYgYGyyg"; //sune
    // char const * cmd_cstr = "GyGYGYGygygg"; //pll ?
    //r U R' U' M2' U R U' R' U' M'
    char const * cmd_cstr = "BRgrbbggOGogogB"; // x oll


    Cmd cmd;

    for (char const * ptr = cmd_cstr; * ptr; ptr ++)
    {
        cmd = Cmd_fromc(* ptr);
        Cmdq_push(& prog->cmdq, & cmd);
    }
    
    prog->cmdq.vel = SPD1;
}

#define CAM_W   3
#define CUBE_W  360

void Prog_input(Prog * prog)
{
    static const KeyboardKey _key_map[BTN_$] =
    {
        [CLR_R] = KEY_R,    [BTN_UP] = KEY_UP,
        [CLR_G] = KEY_G,    [BTN_DOWN] = KEY_DOWN,
        [CLR_O] = KEY_O,    [BTN_LEFT] = KEY_LEFT,
        [CLR_B] = KEY_B,    [BTN_RIGHT] = KEY_RIGHT,
        [CLR_Y] = KEY_Y,    [BTN_SHIFT] = KEY_LEFT_SHIFT,
        [CLR_W] = KEY_W,    [BTN_SPACE] = KEY_SPACE,
                            [BTN_TAB] = KEY_TAB,
                            [BTN_S] = KEY_S,
                            [BTN_T] = KEY_T,
    };

    Cmd cmd = {};

    for (int k = 0; k <= BTN_TAB; k ++)
    {
        prog->input.inputs[k] = IsKeyPressed(_key_map[k]);
    }

    for (int k = BTN_TAB; k < BTN_$; k ++)
    {
        prog->input.inputs[k] = IsKeyDown(_key_map[k]);
    }

    if (WindowShouldClose())                { prog->runs = false; return; }
    if (prog->input.inputs[BTN_LEFT])       _Cam_roty(& prog->cam, CAM_W);
    if (prog->input.inputs[BTN_RIGHT])      _Cam_roty(& prog->cam, -CAM_W);
    if (prog->input.inputs[BTN_SPACE])
    {
        Cmdq_lock_toggle(& prog->cmdq);
    }
    
    // if (Deq_len(& prog->cmdq.deq) > 1)      { return ; }
    for (int k = CLR_R; k < CLR_$; k ++)
    {
        if      (prog->input.inputs[BTN_SHIFT] && prog->input.inputs[k]) cmd = (Cmd) {k, -1};
        else if (prog->input.inputs[k]) cmd = (Cmd) {k, 1};
    }
    
    if (cmd.dir && prog->cmdq.lock)
    {
        Cmdq_reset(& prog->cmdq);
        Cmdq_push(& prog->cmdq, & cmd);
    }
    else if (cmd.dir && Deq_len(& prog->cmdq.deq) < 2)
    { 
        Cmdq_push(& prog->cmdq, & cmd); 
    }
    else if (prog->input.inputs[BTN_SHIFT] && (prog->cmdq.lock || Cmdq_empty(& prog->cmdq)))
    {
        Cmdq_reset(& prog->cmdq);
        printf("calling suffle\n");
        _get_shuffle(prog, 20);
    }
}

void Prog_update(Prog * prog)
{
    Cmd cmd;

    if (Cmdq_poppable(& prog->cmdq) && ! Cube_in_animation(& prog->cube))
    {
        cmd = $drf(Cmd) Deq_popl(& prog->cmdq.deq);
        
        Cube_rot(& prog->cube, cmd.clr, cmd.dir * prog->cmdq.vel);
        Repr_rot(& prog->repr, cmd.clr, cmd.dir * prog->cmdq.vel);

        //
        // printf("CMD :      (%d %d)\n", cmd.clr, cmd.dir);
        // printf("rod score :   %d\n", Repr_score_rod(& prog->repr));
        // printf("bar score : %d\n", Repr_score_bars(& prog->repr));
        // printf("reg score : %d\n", Repr_score_cont_reg(& prog->repr));
        // $nl;

        // printf("test score:   %d\n", Repr_score_test(& prog->repr));
        // printf("dist score : %d\n", Repr_score_distance(& prog->repr));
        // printf("mis score : %d\n", Repr_score_misplaced(& prog->repr));


    }
    else if (prog->input.inputs[BTN_TAB] && ! Cube_in_animation(& prog->cube))
    {
        Cmdq_reset(& prog->cmdq);
        Cube_reset_clr(& prog->cube);
        Repr_init(& prog->repr, CUBE_CLR_STR);
    }
    else if (prog->input.inputs[BTN_S] && ! Cube_in_animation(& prog->cube) && Cmdq_empty(& prog->cmdq))
    {
        Cmdq_reset(& prog->cmdq);

        // Solver_solve(& prog->solver, & prog->repr, & prog->cmd_queue, Repr_score_rod);
        Solver_solve(& prog->solver, & prog->repr, & prog->cmdq.deq, Repr_score_bars);
        // Solver_solve(& prog->solver, & prog->repr, & prog->cmd_queue, Repr_score_cont_reg);

        printf("sln len : %d\n", Deq_len(& prog->cmdq.deq));
    }
    else if (prog->input.inputs[BTN_T] && ! Cube_in_animation(& prog->cube) && Cmdq_empty(& prog->cmdq))
    {
        Cmdq_reset(& prog->cmdq);
        _test(prog);
    }

    Cube_update(& prog->cube);
}

void Prog_draw(Prog * prog)
{
    BeginDrawing();
    ClearBackground(LIGHTGRAY);

    DrawRectangleGradientV(0, 0, WW, WH, DARKGRAY, DARKPURPLE);

    BeginMode3D(prog->cam);

    Cube_draw(& prog->cube);
    EndMode3D();

    DrawFPS(10, 10);
    EndDrawing();
}