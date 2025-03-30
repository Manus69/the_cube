#include "Solver.h"
#include <string.h>

#define LIMIT (1 << 20)

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

        Deq_pushl_check(cmd_queue, & cmd);
        idx = parent_idx;
    }

    return Deq_len(cmd_queue);
}

static int _add_repr(Solver * solver, Repr const * repr, int parent, Cmd cmd)
{
    IdScore ids;

    if (Htbl_get(& solver->tbl, repr, Repr_hashf, Repr_eqf))    return 0;

    ids = (IdScore)
    {
        .id = Vec_len(& solver->reprs),
        // .score = Repr_score_cum(repr),
        // .score = Repr_score_nbghr(repr),
        .score = Repr_score_rod(repr),
    };

    if (! Vec_push_check(& solver->reprs, repr))                return NO_IDX;
    if (! Vec_push_check(& solver->scores, & ids.score))        return NO_IDX;
    if (! Vec_push_check(& solver->cmds, & cmd))                return NO_IDX;
    if (! Vec_push_check(& solver->parents, & parent))          return NO_IDX;
    if (! Htbl_insert_check(& solver->tbl, repr, Repr_hashf))   return NO_IDX;
    if (! Heap_insert_check(& solver->queue, & ids, IdScore_cmpf, IdScore_swapf))
        return NO_IDX;

    return 1;
}

static int _solve(Solver * solver, Deq * cmd_queue)
{
    IdScore ids;
    Repr *  current;
    Repr    next;

    while (true)
    {
        if (Vec_len(& solver->reprs) > LIMIT)   return NO_IDX;
        if (Heap_empty(& solver->queue))        return NO_IDX;

        ids = $drf(IdScore) Heap_pop_top(& solver->queue, IdScore_cmpf, IdScore_swapf);
        current = Vec_get(& solver->reprs, ids.id);

        //
        printf("%d\n", ids.id);

        if (Repr_solved(current))
        {
            return _backtrack(solver, ids.id, cmd_queue);
        }
    
        for (CLR clr = 0; clr < CLR_$; clr ++)
        {
            for (int dir = -1; dir <= 1; dir += 2)
            {
                Repr_next(& next, current, clr, dir);
                if (_add_repr(solver, & next, ids.id, (Cmd) {clr, dir}) < 0) return NO_IDX;
            }
        }
    }
}

static void _reset(Solver * solver)
{
    Vec_pop_all(& solver->reprs);
    Vec_pop_all(& solver->parents);
    Vec_pop_all(& solver->cmds);
    Vec_pop_all(& solver->scores);
    Heap_pop_all(& solver->queue);
    Htbl_purge(& solver->tbl);
}

int Solver_solve(Solver * solver, Repr const * repr, Deq * cmd_queue)
{
    _reset(solver);

    if (_add_repr(solver, repr, NO_IDX, (Cmd) {NO_IDX, NO_IDX}) < 0) return NO_IDX;

    return _solve(solver, cmd_queue);
}

#define _DC (1 << 18)
bool Solver_new(Solver * solver)
{
    return  Vec_new_capacity(& solver->reprs, sizeof(Repr), _DC) && 
            Vec_new_capacity(& solver->parents, sizeof(int), _DC) &&
            Vec_new_capacity(& solver->scores, sizeof(int), _DC) &&
            Vec_new_capacity(& solver->cmds, sizeof(Cmd), _DC) &&
            Heap_new_capacity(& solver->queue, sizeof(IdScore), _DC) &&
            Htbl_new_capacity(& solver->tbl, sizeof(Repr), _DC);
}

void Solver_del(Solver * solver)
{
    Vec_del(& solver->reprs);
    Vec_del(& solver->parents);
    Vec_del(& solver->scores);
    Vec_del(& solver->cmds);
    Heap_del(& solver->queue);
    Htbl_del(&  solver->tbl);
}