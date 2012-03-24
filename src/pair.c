#include <stdbool.h>
#include "utils.h"
#include "pair.h"

struct Pair {
  Expression *fst, *snd;
  bool flag;
};

Pair *
Pair_new (Expression *fst, Expression *snd)
{
  Pair *self = NULL;

  alloc_one(self);

  self->fst = fst;
  self->snd = snd;
  self->flag = false;

  return self;
}

void
Pair_delete (Pair *self)
{
  if (self == NULL) return;
  Expression_delete(self->fst);
  Expression_delete(self->snd);
  free_(self);
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

void
Pair_setFst (Pair *self, Expression *expr)
{
  self->fst = expr;
}

void
Pair_setSnd (Pair *self, Expression *expr)
{
  self->snd = expr;
}

bool
Pair_flag (Pair *self)
{
  return self->flag;
}

void
Pair_setFlag (Pair *self, bool flag)
{
  self->flag = flag;
}
