#ifndef SOLVER_H
#define SOLVER_H

#include "Repr.h"

typedef struct
{
    Vec     reprs;
    Vec     parents;
    Vec     cmds;
    Heap    queue;
    Htbl    tbl;
}   Solver;

int     Solver_solve(Solver * solver, Repr const * repr, Deq * cmd_queue);
bool    Solver_new(Solver * solver);
void    Solver_del(Solver * solver);

#endif