#include "utils.h"
#include "expression.h"
#include "vector.h"

struct Vector {
  int size;
  Expression **fields;
};

Vector *
Vector_new (int size)
{
  Vector *self = NULL;

  alloc_one(self);

  self->size = size;
  alloc_(self->fields, size);

  return self;
}

void
Vector_delete (Vector *self)
{
  int i;

  if (self == NULL) return;
  for (i = 0; i < self->size; i++)
    Expression_delete(self->fields[i]);
  free_(self);
}

/* ----- */

int
Vector_size (Vector *self)
{
  return self->size;
}

/* ----- */

void
Vector_set (Vector *self, int i, Expression *expr)
{
  self->fields[i] = expr;
}

Expression *
Vector_get (Vector *self, int i)
{
  return self->fields[i];
}
