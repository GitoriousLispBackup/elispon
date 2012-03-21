#include <stdbool.h>
#include "utils.h"
#include "expression.h"
#include "pair.h"
#include "symbol.h"
#include "string.h"
#include "number.h"

struct Expression {
  ExprType type;
  void *expr;
};

Expression *
Expression_new (ExprType type, void *expr)
{
  Expression *self = NULL;

  debug_in();

  alloc_one(self);

  self->type = type;
  self->expr = expr;

  debug_out();

  return self;
}

void
Expression_delete (Expression *self)
{
  debug_in();

  switch (self->type) {
  case PAIR:
    if (self->expr != NULL) Pair_delete(self->expr);
    break;
  case SYMBOL:
    Symbol_delete(self->expr);
    break;
  case STRING:
    String_delete(self->expr);
    break;
  case NUMBER:
    Number_delete(self->expr);
    break;
  default:
    Utils_error("Expression_delete: unknown expression type.");
  }

  free_(self);

  debug_out();
}

/* ----- */

ExprType
Expression_type (Expression *self)
{
  return self->type;
}

void *
Expression_expr (Expression *self)
{
  return self->expr;
}

/* ----- */

bool
Expression_isNil (Expression *self)
{
  return self->type == PAIR
    && !self->expr;
}

bool
Expression_isAtom (Expression *self)
{
  return self->type != PAIR;
}

bool
Expression_isValue (Expression *self)
{
  return self->type == NUMBER
    || self->type == STRING
    || Expression_isNil(self);
}
