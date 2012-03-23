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

/* ----- */

Number *
Number_add (Number *self, Number *num)
{
  return Number_new(self->val + num->val);
}

Number *
Number_sub (Number *self, Number *num)
{
  return Number_new(self->val - num->val);
}

Number *
Number_mul (Number *self, Number *num)
{
  return Number_new(self->val * num->val);
}

Number *
Number_div (Number *self, Number *num)
{
  return Number_new(self->val / num->val);
}

#define round(x) ((x)>=0?(long)((x)+0.5):(long)((x)-0.5))

Number *
Number_idiv (Number *self, Number *num)
{
  return Number_new(round(self->val) / round(num->val));
}

Number *
Number_mod (Number *self, Number *num)
{
  return Number_new(round(self->val) % round(num->val));
}

#undef round
