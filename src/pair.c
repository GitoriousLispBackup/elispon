#include "utils.h"
#include "pair.h"

struct Pair {
  Expression *fst, *snd;
};

Pair *
Pair_new (Expression *fst, Expression *snd)
{
  Pair *self = NULL;

  debug_in();

  alloc_one(self);

  self->fst = fst;
  self->snd = snd;

  debug_out();

  return self;
}

void
Pair_delete (Pair *self)
{
  debug_in();
  
  Expression_delete(self->fst);
  Expression_delete(self->snd);
  free_(self);

  debug_out();
}

/* ----- */

Expression *
Pair_fst (Pair *self)
{
  return self->fst;
}

Expression *
Pair_snd (Pair *self)
{
  return self->snd;
}
