#include <stdbool.h>
#include "utils.h"
#include "expression.h"
#include "primitive.h"
#include "pair.h"
#include "symbol.h"
#include "string.h"
#include "number.h"
#include "fexpr.h"

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
  static int i = 0;
  int j;

  if (self == NULL) return;
  for (j = 0; j < i; j++) printf("  ");
  switch (self->type) {
  case PRIMITIVE:
    printf("deleting primitive %s\n", Primitive_name(self->expr));
    break;
  case PAIR:
    printf("deleting pair %d\n", i);
    if (self->expr != NULL) {
      i++;
      Pair_delete(self->expr);
      i--;
    }
    break;
  case SYMBOL:
    printf("deleting symbol %s\n", Symbol_name(self->expr));
    Symbol_delete(self->expr);
    break;
  case STRING:
    printf("deleting string %s\n", String_buf(self->expr));
    String_delete(self->expr);
    break;
  case NUMBER:
    printf("deleting number %g\n", Number_val(self->expr));
    Number_delete(self->expr);
    break;
  case FEXPR:
    printf("deleting fexpr\n");
    Fexpr_delete(self->expr);
  default:
    printf("fatal\n");
    Utils_fatal("%d Expression_delete: unknown expression type.", i);
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
Expression_isCallable (Expression *self)
{
  return self->type == PRIMITIVE || self->type == FEXPR;
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

void
Expression_setCar (Expression *self, Expression *expr)
{
  if (Expression_type(self) != PAIR || Expression_expr(self) == NULL)
    Utils_fatal("Expression_setCar: argument is not a pair");
  Pair_setFst(Expression_expr(self), expr);
}

void
Expression_setCdr (Expression *self, Expression *expr)
{
  if (Expression_type(self) != PAIR || Expression_expr(self) == NULL)
    Utils_fatal("Expression_setCdr: argument is not a pair");
  Pair_setSnd(Expression_expr(self), expr);
}

int
Expression_length (Expression *self)
{
  int l = 0;

  if (Expression_type(self) != PAIR)
    return 0;

  while (Expression_type(self) == PAIR && self->expr != NULL) {
    l++;
    self = Expression_cdr(self);
  }

  return l;
}
