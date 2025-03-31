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

int     Solver_solve(Solver * solver, Repr const * repr, Deq * cmd_queue, int (* f)(Repr const *));
bool    Solver_new(Solver * solver);
void    Solver_del(Solver * solver);

typedef struct
{
    Solver  fsolver;
    Solver  bsolver;
}   SolverM;

int     SolverM_solve(SolverM * solver, Repr const * repr, Deq * cmd_queue, int (* f)(Repr const *));
bool    SolverM_new(SolverM * solver);
void    SolverM_del(SolverM * solver);

#endif