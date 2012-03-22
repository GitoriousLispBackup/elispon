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
  bool error;
};

Eval *
Eval_new ()
{
  Eval *self = NULL;

  alloc_one(self);

  self->primitives = Primitive_initialEnvironment();
  self->error = false;

  return self;
}

void
Eval_delete (Eval *self)
{
  if (self == NULL) return;
  Environment_delete(self->primitives);
  free_(self);
}

void
Eval_reset (Eval *self)
{
  self->error = false;
}

/* ----- */

static Expression *
Eval_evalPair (Eval *self, Pair *pair, Environment **env)
{
  Expression *car = NULL, *expr = NULL;

  car = Eval_eval(self, Pair_fst(pair), env);
  if (self->error)
    return NULL;
  if (!Expression_isCallable(car)) {
    Utils_error("expected procedure");
    return NULL;
  }

  expr = Primitive_proc(Expression_expr(car))(Pair_snd(pair), env);
  if (expr == NULL)
    self->error = true;

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
                                  Expression_expr(expr))) == NULL) {
          Utils_error("%s: undefined symbol",
                      Symbol_name(Expression_expr(expr)));
          self->error = true;
        }

      return e;
    }
  case PRIMITIVE:
  case STRING:
  case NUMBER:
    return expr;
  default:
    Utils_error("Eval_eval: unknown expression type");
    self->error = true;
  }

  return NULL;
}

/* ----- */

Expression *
Eval_eval (Eval *self, Expression *expr, Environment **env)
{
  while (!self->error && !Expression_isValue(expr))
    expr = Eval_evalExpression(self, expr, env);
  return expr;
}
