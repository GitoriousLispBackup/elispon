#include "utils.h"
#include "number.h"

struct Number {
  double val;
};

Number *
Number_new (double val)
{
  Number *self = NULL;

  debug_in();

  alloc_one(self);

  self->val = val;

  debug_out();

  return self;
}

void
Number_delete (Number *self)
{
  debug_in();

  free_(self);

  debug_out();
}

/* ----- */

double
Number_val (Number *self)
{
  return self->val;
}
