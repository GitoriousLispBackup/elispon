#include <stdbool.h>
#include "utils.h"
#include "primitive.h"
#include "pair.h"
#include "symbol.h"
#include "character.h"
#include "string.h"
#include "number.h"
#include "fexpr.h"
#include "vector.h"
#include "environment.h"
#include "struct.h"
#include "object.h"
#include "expression.h"

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
  case NIL:
    break;
  case PAIR:
    if (self->expr != NULL)
      Pair_delete(self->expr);
    break;
  case SYMBOL:
    Symbol_delete(self->expr);
    break;
  case CHARACTER:
    Character_delete(self->expr);
    break;
  case STRING:
    String_delete(self->expr);
    break;
  case NUMBER:
    Number_delete(self->expr);
    break;
  case FEXPR:
    Fexpr_delete(self->expr);
    break;
  case VECTOR:
    Vector_delete(self->expr);
    break;
  case ENVIRONMENT:
    Environment_delete(self->expr);
    break;
  case STRUCT:
    Struct_delete(self->expr);
    break;
  case OBJECT:
    Object_delete(self->expr);
    break;
  default:
    Utils_fatal("Expression_delete: unknown expression type.");
  }
  free_(self);
}

/* ----- */

ExprType
Expression_type (Expression *self)
{
  return self->type;
}

void
Expression_setType (Expression *self, ExprType type)
{
  self->type = type;
}

void *
Expression_expr (Expression *self)
{
  return self->expr;
}

void
Expression_setExpr (Expression *self, void *expr)
{
  self->expr = expr;
}

/* ----- */

char *
Expression_typeName (Expression *self)
{
  switch (self->type) {
  case PRIMITIVE:
    return "primitive";
  case NIL:
    return "nil";
  case PAIR:
    return "pair";
  case SYMBOL:
    return "symbol";
  case CHARACTER:
    return "character";
  case STRING:
    return "string";
  case NUMBER:
    return "number";
  case FEXPR:
    return "fexpr";
  case VECTOR:
    return "vector";
  case ENVIRONMENT:
    return "environment";
  case STRUCT:
    return "struct";
  case OBJECT:
    return "object";
  default:
    return "unknown";
  }
}

Expression *
Expression_cons (Expression *car, Expression *cdr)
{
  return Expression_new(PAIR, Pair_new(car, cdr));
}

Expression *
Expression_car (Expression *self)
{
  if (self->type != PAIR)
    Utils_fatal("Expression_car: argument is not a pair.");

  return Pair_fst(self->expr);
}

Expression *
Expression_cdr (Expression *self)
{
  if (self->type != PAIR)
    Utils_fatal("Expression_car: argument is not a pair.");

  return Pair_snd(self->expr);
}

void
Expression_setCar (Expression *self, Expression *expr)
{
  if (self->type != PAIR)
    Utils_fatal("Expression_setCar: argument is not a pair.");
  Pair_setFst(self->expr, expr);
}

void
Expression_setCdr (Expression *self, Expression *expr)
{
  if (self->type != PAIR)
    Utils_fatal("Expression_setCdr: argument is not a pair.");
  Pair_setSnd(self->expr, expr);
}

int
Expression_length (Expression *self)
{
  Expression *list = NULL;
  int l = 0;

  if (self->type != PAIR)
    return 0;

  list = self;
  while (list->type == PAIR) {
    if (Pair_flag(list->expr)) {
      l = -1;
      break;
    }
    l++;
    Pair_setFlag(list->expr, true);
    list = Expression_cdr(list);
  }

  list = self;
  while (list->type == PAIR) {
    if (!Pair_flag(list->expr)) break;
    Pair_setFlag(list->expr, false);
    list = Expression_cdr(list);
  }

  return l;
}
