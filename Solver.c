#include "Solver.h"
#include <string.h>

#define LIMIT (1 << 20)
#define DEPTH 200

typedef struct
{
    int id;
    int score;
}   IdScore;

bool Repr_solved(Repr const * repr)
{
    for (int k = 0; k < REPR_LEN; k ++)
    {
        if (repr->buff[k] != CUBE_CLR_STR[k]) return false;
    }

    return true;
}

void Repr_next(Repr * restrict dst, Repr const * src, CLR clr, int dir)
{
    memcpy(dst->buff, src->buff, REPR_LEN);
    Repr_rot(dst, clr, dir);
}

static bool Repr_eq(Repr const * lhs, Repr const * rhs)
{
    for (int k = 0; k < REPR_LEN; k ++)
    {
        if (lhs->buff[k] != rhs->buff[k]) return false;
    }

    return true;
}

static bool Repr_eqf(void const * lhs, void const * rhs)
{
    return Repr_eq(lhs, rhs);
}

static int IdScore_cmp(IdScore const * lhs, IdScore const * rhs)
{
    return lhs->score - rhs->score;
}

static int IdScore_cmpf(void const * lhs, void const * rhs)
{
    return IdScore_cmp(lhs, rhs);
}

$swapf_gen(IdScore)

#include <stdio.h>
static int _backtrack(Solver * solver, int idx, Deq * cmd_queue)
{
    Cmd cmd;
    int parent_idx;

    while (true)
    {
        parent_idx = $drf(int) Vec_get(& solver->parents, idx);
        cmd = $drf(Cmd) Vec_get(& solver->cmds, idx);

        if (cmd.clr == NO_IDX) break;

        Deq_pushl(cmd_queue, & cmd);
        idx = parent_idx;
    }

    return Deq_len(cmd_queue);
}

static int _push(Solver * solver, Repr const * repr, Cmd cmd, int parent, int depth, IdScore ids)
{
    if (! Vec_push(& solver->reprs, repr))                return NO_IDX;
    if (! Vec_push(& solver->cmds, & cmd))                return NO_IDX;
    if (! Vec_push(& solver->parents, & parent))          return NO_IDX;
    if (! Vec_push(& solver->depth, & depth))             return NO_IDX;
    if (! Htbl_ins(& solver->tbl, repr, Repr_hashf))      return NO_IDX;
    if (! Heap_ins(& solver->queue, & ids, IdScore_cmpf, IdScore_swapf))
        return NO_IDX;

    return 1;
}

static int _add_repr(Solver * solver, Repr const * repr, int parent, int depth, Cmd cmd, int (* f)(Repr const *))
{
    IdScore ids;

    if (Htbl_get(& solver->tbl, repr, Repr_hashf, Repr_eqf))    return 0;

    ids = (IdScore)
    {
        .id = Vec_len(& solver->reprs),
        .score = f(repr),
        // .score = f(repr) * (depth + 1),
    };

    return _push(solver, repr, cmd, parent, depth, ids);
}

static int _solve(Solver * solver, Deq * cmd_queue, int (* f)(Repr const *))
{
    IdScore ids;
    Repr *  current;
    Repr    next;
    int     depth;

    while (true)
    {
        if (Vec_len(& solver->reprs) > LIMIT)   return NO_IDX;
        if (Heap_empty(& solver->queue))        return NO_IDX;

        ids = $drf(IdScore) Heap_pop(& solver->queue, IdScore_cmpf, IdScore_swapf);
        current = Vec_get(& solver->reprs, ids.id);
        depth = $drf(int) Vec_get(& solver->depth, ids.id);

        
        if (depth > DEPTH) continue;
        if (Repr_solved(current))
        {
            //
            printf("sln: id %d, depth: %d\n", ids.id, depth);
            return _backtrack(solver, ids.id, cmd_queue);
        }
        

        for (CLR clr = 0; clr < CLR_$; clr ++)
        {
            for (int dir = -1; dir <= 1; dir += 2)
            {
                Repr_next(& next, current, clr, dir);
                if (_add_repr(solver, & next, ids.id, depth + 1, (Cmd) {clr, dir}, f) < 0) return NO_IDX;
            }
        }
    }
}

static void _reset(Solver * solver)
{
    Vec_pop_all(& solver->reprs);
    Vec_pop_all(& solver->parents);
    Vec_pop_all(& solver->cmds);
    Heap_pop_all(& solver->queue);
    Htbl_purge(& solver->tbl);
}

int Solver_solve(Solver * solver, Repr const * repr, Deq * cmd_queue, int (* f)(Repr const *))
{
    _reset(solver);

    if (_add_repr(solver, repr, NO_IDX, 0, (Cmd) {NO_IDX, NO_IDX}, f) < 0) return NO_IDX;

    return _solve(solver, cmd_queue, f);
}

#define _DC (1 << 18)
bool Solver_new(Solver * solver)
{
    return  Vec_new_capacity(& solver->reprs,  _DC, sizeof(Repr)) && 
            Vec_new_capacity(& solver->parents, _DC,  sizeof(int)) &&
            Vec_new_capacity(& solver->cmds, _DC,  sizeof(Cmd)) &&
            Vec_new_capacity(& solver->depth, _DC,  sizeof(int)) && 
            Heap_new_capacity(& solver->queue, _DC, sizeof(IdScore)) &&
            Htbl_new_capacity(& solver->tbl,  _DC, sizeof(Repr));
}

void Solver_del(Solver * solver)
{
    Vec_del(& solver->reprs);
    Vec_del(& solver->parents);
    Vec_del(& solver->cmds);
    Vec_del(& solver->depth);
    Heap_del(& solver->queue);
    Htbl_del(&  solver->tbl);
}

//-----------

static int _backtrackM(SolverM * solver, int fid, Deq * cmd_queue)
{
    Cmd cmd;
    int parent_idx;
    int idx;
    
    _backtrack(& solver->fsolver, fid, cmd_queue);

    idx = Vec_idxof(& solver->bsolver.reprs, Vec_get(& solver->fsolver.reprs, fid), Repr_eqf);

    while (true)
    {
        parent_idx = $drf(int) Vec_get(& solver->bsolver.parents, idx);
        cmd = $drf(Cmd) Vec_get(& solver->bsolver.cmds, idx);

        if (cmd.clr == NO_IDX) break;

        cmd.dir *= -1;
        Deq_pushr(cmd_queue, & cmd);
        idx = parent_idx;
    }

    return Deq_len(cmd_queue);
}

static int _add_reprM(Solver * solver, Repr const * repr, int parent, int depth, Cmd cmd, Repr const * start)
{
    IdScore ids;

    if (Htbl_get(& solver->tbl, repr, Repr_hashf, Repr_eqf))    return 0;

    ids = (IdScore)
    {
        .id = Vec_len(& solver->reprs),
        .score = Repr_score_dist(repr, start->buff),
        // .score = 0,
    };

    return _push(solver, repr, cmd, parent, depth, ids);
}

static int _solveM(SolverM * solver, Deq * cmd_queue, int (* f)(Repr const *))
{
    IdScore fids, bids;
    Repr *  fcurrent, * bcurrent;
    Repr    fnext, bnext;
    int     fdepth, bdepth;

    while (true)
    {
        if (Vec_len(& solver->fsolver.reprs) > LIMIT)   return NO_IDX;
        if (Heap_empty(& solver->fsolver.queue))        return NO_IDX;
        if (Heap_empty(& solver->bsolver.queue))        return NO_IDX;

        fids = $drf(IdScore) Heap_pop(& solver->fsolver.queue, IdScore_cmpf, IdScore_swapf);
        bids = $drf(IdScore) Heap_pop(& solver->bsolver.queue, IdScore_cmpf, IdScore_swapf);

        fcurrent = Vec_get(& solver->fsolver.reprs, fids.id);
        bcurrent = Vec_get(& solver->bsolver.reprs, bids.id);

        fdepth = $drf(int) Vec_get(& solver->fsolver.depth, fids.id);
        bdepth = $drf(int) Vec_get(& solver->bsolver.depth, bids.id);

        if (fdepth + bdepth > DEPTH) continue;

        // if (Htbl_get(& solver->bsolver.tbl, fcurrent, Repr_hashf, Repr_eqf) || Htbl_get(& solver->fsolver.tbl, bcurrent, Repr_hashf, Repr_eqf))
        if (Htbl_get(& solver->bsolver.tbl, fcurrent, Repr_hashf, Repr_eqf))
        {
            printf("\n%d %d\n", fids.id, bids.id);

            return _backtrackM(solver, fids.id, cmd_queue);
        }

        for (CLR clr = 0; clr < CLR_$; clr ++)
        {
            for (int dir = -1; dir <= 1; dir += 2)
            {
                Repr_next(& fnext, fcurrent, clr, dir);
                Repr_next(& bnext, bcurrent, clr, dir);

                if (_add_repr(& solver->fsolver, & fnext, fids.id, fdepth + 1, (Cmd) {clr, dir}, f) < 0) return NO_IDX;
                if (_add_reprM(& solver->bsolver, & bnext, bids.id, bdepth + 1, (Cmd) {clr, dir}, Vec_get(& solver->fsolver.reprs, 0)) < 0) return NO_IDX;
                // if (_add_reprM(& solver->bsolver, & bnext, bids.id, (Cmd) {clr, dir}, fcurrent) < 0) return NO_IDX;
                
            }
        }
    }
}

static void _resetM(SolverM * solver)
{
    _reset(& solver->bsolver);
    _reset(& solver->fsolver);
}

int SolverM_solve(SolverM * solver, Repr const * repr, Deq * cmd_queue, int (* f)(Repr const *))
{
    Repr solved;

    _resetM(solver);

    Repr_init(& solved, CUBE_CLR_STR);

    if (_add_repr(& solver->fsolver, repr, NO_IDX, 0, (Cmd) {NO_IDX, NO_IDX}, f) < 0)          return NO_IDX;
    if (_add_reprM(& solver->bsolver, & solved, NO_IDX, 0, (Cmd) {NO_IDX, NO_IDX}, repr) < 0)  return NO_IDX;

    return _solveM(solver, cmd_queue, f);
}

bool SolverM_new(SolverM * solver)
{
    return Solver_new(& solver->fsolver) && Solver_new(& solver->bsolver);
}

void SolverM_del(SolverM * solver)
{
    Solver_del(& solver->fsolver);
    Solver_del(& solver->bsolver);
}