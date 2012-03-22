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

  alloc_one(self);

  self->type = type;
  self->expr = expr;

  return self;
}

void
Expression_delete (Expression *self)
{
  if (self == NULL) return;
  switch (self->type) {
  case PRIMITIVE:
    break;
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
  return self->type == PAIR && !self->expr;
}

bool
Expression_isAtom (Expression *self)
{
  return self->type != PAIR;
}

bool
Expression_isValue (Expression *self)
{
  return self->type == NUMBER || self->type == STRING ||
    self->type == PRIMITIVE || Expression_isNil(self);
}

bool
Expression_isCallable (Expression *self)
{
  return self->type == PRIMITIVE;
}

Expression *
Expression_cons (Expression *car, Expression *cdr)
{
  return Expression_new(PAIR, Pair_new(car, cdr));
}

Expression *
Expression_car (Expression *self)
{
  if (Expression_type(self) != PAIR || Expression_expr(self) == NULL)
    Utils_fatal("Expression_car: argument is not a pair");

  return Pair_fst(Expression_expr(self));
}

Expression *
Expression_cdr (Expression *self)
{
  if (Expression_type(self) != PAIR || Expression_expr(self) == NULL)
    Utils_fatal("Expression_car: argument is not a pair");

  return Pair_snd(Expression_expr(self));
}
