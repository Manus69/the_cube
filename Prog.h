#ifndef PROG_H
#define PROG_H

#include "master.h"

typedef struct Prog Prog;

Prog *  Prog_new(void);
void    Prog_del(Prog * prog);
bool    Prog_runs(Prog const * prog);
void    Prog_input(Prog * prog);
void    Prog_update(Prog * prog);
void    Prog_draw(Prog * prog);

#endif