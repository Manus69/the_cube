#define _SFLIB_IMPL
#include "../_sflib/_sflib_cont.h"
#include "Prog.h"

//icon
//redo solver
//prune
//investigate layers
//investigate the mysterious segfault
//scaling score slice
//dont add inv parent move

//lib ringbuffer

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
