#include "Prog.h"
#include "Cube.h"
#include <stdlib.h>

#define WN      "The Cube"
#define WW      800
#define WH      600
#define TFPS    60

struct Prog
{
    Cube    cube;
    Camera  cam;
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
    prog->cam.position = (Vector3){13, 13, 13};
    prog->cam.target = (Vector3){};
    prog->cam.up = (Vector3){0, 1, 0};
    prog->cam.fovy = 45;
    prog->cam.projection = CAMERA_PERSPECTIVE;
}

Prog * Prog_new(void)
{
    Prog * prog;

    if (! (prog = calloc(1, sizeof(* prog))))       return NULL;
    if (! Deq_new(& prog->cmd_queue, sizeof(Cmd)))  return NULL;

    _graphics_init(prog);
    _Cam_init(prog);
    Cube_init(& prog->cube);

    prog->runs = true;

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

void Prog_input(Prog * prog)
{

}

void Prog_update(Prog * prog)
{

}

void Prog_draw(Prog * prog)
{
    
}