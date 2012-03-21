#include "utils.h"
#include "number.h"

struct Number {
  double val;
};

Number *
Number_new (double val)
{
  Number *self = NULL;

  alloc_one(self);

  self->val = val;

  return self;
}

void
Number_delete (Number *self)
{
  if (self == NULL) return;
  free_(self);
}

/* ----- */

double
Number_val (Number *self)
{
  return self->val;
}
