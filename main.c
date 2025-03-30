#define _SLIB_IMPL
#include "../_slib/_slib.h"
#include "Prog.h"

// blocks get misaligned at higher fps
// redo animation (int n steps, fixed dx)

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
