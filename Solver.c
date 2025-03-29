#include "Solver.h"
#include <string.h>

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

int Repr_score(Repr const * repr)
{
    int score;

    score = 0;
    for (int k = 0; k < REPR_LEN; k ++)
    {
        score += (repr->buff[k] != CUBE_CLR_STR[k]); 
    }

    return score;
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

static int _add_repr(Solver * solver, Repr const * repr)
{
    IdScore ids;

    if (Htbl_get(& solver->tbl, repr, Repr_hashf, Repr_eqf))    return 0;

    ids = (IdScore)
    {
        .id = 0,
        .score = Repr_score(repr),
    };

    if (! Vec_push_check(& solver->reprs, repr))                return NO_IDX;
    if (! Vec_push_check(& solver->scores, & ids.score))        return NO_IDX;
    if (! Vec_push_check(& solver->parents, & (int){ NO_IDX })) return NO_IDX;
    if (! Heap_insert_check(& solver->queue, & ids, IdScore_cmpf, IdScore_swapf))
        return NO_IDX;

    return 1;
}

static int _visit(Solver * solver)
{
    IdScore ids;
    Repr *  current;
    Repr    next;

    ids = $drf(IdScore) Heap_pop_top(& solver->queue, IdScore_cmpf, IdScore_swapf);
    current = Vec_get(& solver->reprs, ids.id);

    if (Repr_solved(current))
    {
        //
        Repr_clr_dbg(current);
        return 0;
    }

    for (CLR clr = 0; clr < CLR_$; clr ++)
    {
        for (int dir = -1; dir <= 1; dir += 2)
        {
            Repr_next(& next, current, clr, dir);
            if (_add_repr(solver, & next) < 0) return NO_IDX;
        }
    }

    return _visit(solver);
}

int Solver_solve(Solver * solver, Repr const * repr)
{
    if (_add_repr(solver, repr) < 0) return NO_IDX;

    return _visit(solver);
}

bool Solver_new(Solver * solver)
{
    return  Vec_new(& solver->reprs, sizeof(Repr)) && 
            Vec_new(& solver->parents, sizeof(int)) &&
            Vec_new(& solver->scores, sizeof(int)) &&
            Heap_new(& solver->queue, sizeof(IdScore)) &&
            Htbl_new(& solver->tbl, sizeof(Repr));
}

void Solver_del(Solver * solver)
{
    Vec_del(& solver->reprs);
    Vec_del(& solver->parents);
    Vec_del(& solver->scores);
    Heap_del(& solver->queue);
    Htbl_del(&  solver->tbl);
}