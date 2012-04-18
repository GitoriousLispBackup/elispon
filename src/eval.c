#include <stdlib.h>
#include <stdbool.h>
#include "utils.h"
#include "expression.h"
#include "primitive.h"
#include "pair.h"
#include "symbol.h"
#include "character.h"
#include "string.h"
#include "number.h"
#include "fexpr.h"
#include "environment.h"
#include "struct.h"
#include "eval.h"

struct Eval {
  Port *errput;
};

Eval *
Eval_new (Port *errput)
{
  Eval *self = NULL;

  alloc_one(self);

  self->errput = errput;

  return self;
}

void
Eval_delete (Eval *self)
{
  if (self == NULL) return;
  free_(self);
}

/* ----- */

Port *
Eval_errput (Eval *self)
{
  return self->errput;
}

/* ----- */

static Expression *
Eval_evalPair (Eval *self, Pair *pair, Environment **env)
{
  Expression *f = NULL;

  if ((f = Eval_eval(self, Pair_fst(pair), env)) == NULL)
    return NULL;
  if (!Expression_isCallable(f)) {
    Utils_error("expected primitive or fexpr, given a %s",
                Expression_typeName(f));
    return NULL;
  }

  if (Expression_type(f) == PRIMITIVE)
    return Primitive_proc(Expression_expr(f))(Pair_snd(pair), env, self);

  if (Expression_type(f) == FEXPR) {
    Environment *environment = NULL;

    environment = Fexpr_lexenv(Expression_expr(f));
    Environment_add(environment, Fexpr_arg(Expression_expr(f)),
                    Pair_snd(pair));
    Environment_add(environment, Fexpr_dynenv(Expression_expr(f)),
                    Expression_new(ENVIRONMENT, *env));

    return Eval_eval(self, Fexpr_body(Expression_expr(f)), &environment);
  }

  return NULL;
}

static Expression *
Eval_evalExpression (Eval *self, Expression *expr, Environment **env)
{
  switch (Expression_type(expr)) {
  case PAIR:
    return Eval_evalPair(self, Expression_expr(expr), env);
  case SYMBOL:
    {
      Expression *e = NULL;
      if ((e = Environment_find(*env, Expression_expr(expr))) == NULL)
        Utils_error("%s: undefined symbol",
                    Symbol_name(Expression_expr(expr)));
      return e;
    }
  case PRIMITIVE:
  case NIL:
  case CHARACTER:
  case STRING:
  case NUMBER:
  case FEXPR:
  case ENVIRONMENT:
  case STRUCT:
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
