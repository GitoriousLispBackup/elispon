#include <stdlib.h>
#include <stdbool.h>
#include "utils.h"
#include "expression.h"
#include "pair.h"
#include "symbol.h"
#include "string.h"
#include "number.h"
#include "primitive.h"
#include "environment.h"
#include "eval.h"

struct Eval {
  Environment *primitives;
};

Eval *
Eval_new (Environment *primitives)
{
  Eval *self = NULL;

  alloc_one(self);

  self->primitives = primitives;

  return self;
}

void
Eval_delete (Eval *self)
{
  if (self == NULL) return;
  free_(self);
}

/* ----- */

static Expression *
Eval_evalPair (Eval *self, Pair *pair, Environment **env)
{
  Expression *car = NULL, *expr = NULL;

  if ((car = Eval_eval(self, Pair_fst(pair), env)) == NULL)
    return NULL;
  if (!Expression_isCallable(car)) {
    Utils_error("expected procedure");
    return NULL;
  }

  expr = Primitive_proc(Expression_expr(car))(Pair_snd(pair), env, self);

  return expr;
}

static Expression *
Eval_evalExpression (Eval *self, Expression *expr, Environment **env)
{
  switch (Expression_type(expr)) {
  case PAIR:
    if (Expression_isNil(expr))
      return expr;
    return Eval_evalPair(self, Expression_expr(expr), env);
  case SYMBOL:
    {
      Expression *e = NULL;

      if ((e = Environment_find(*env, Expression_expr(expr))) == NULL)
        if ((e = Environment_find(self->primitives,
                                  Expression_expr(expr))) == NULL)
          Utils_error("%s: undefined symbol",
                      Symbol_name(Expression_expr(expr)));

      return e;
    }
  case PRIMITIVE:
  case STRING:
  case NUMBER:
    return expr;
  default:
    Utils_error("Eval: unknown expression type");
  }

  return NULL;
}

/* ----- */

Expression *
Eval_eval (Eval *self, Expression *expr, Environment **env)
{
  return Eval_evalExpression(self, expr, env);
}

Expression *
Eval_mapEval (Eval *self, Expression *expr, Environment **env)
{
  Expression *car = NULL, *list = expr;

  while (!Expression_isNil(list)) {
    if (Expression_type(list) != PAIR) {
      Utils_error("expected pair");
      return NULL;
    }
    if ((car = Eval_eval(self, Expression_car(list), env)) == NULL)
      return NULL;

    Expression_setCar(list, car);
    list = Expression_cdr(list);
  }

  return expr;
}
