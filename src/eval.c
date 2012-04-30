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
#include "object.h"
#include "eval.h"

struct Eval {
  Port *input, *output, *errput;
};

Eval *
Eval_new (Port *input, Port *output, Port *errput)
{
  Eval *self = NULL;

  alloc_one(self);

  self->input = input;
  self->output = output;
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
Eval_input (Eval *self)
{
  return self->input;
}

Port *
Eval_output (Eval *self)
{
  return self->output;
}

Port *
Eval_errput (Eval *self)
{
  return self->errput;
}

/* ----- */

static Expression *
Eval_applyFexpr (Eval *self, Fexpr *f, Expression *args, Environment **env)
{
  Environment *environment = NULL;

  environment = Fexpr_lexenv(f);
  Environment_add(environment, Fexpr_arg(f), args);
  Environment_add(environment, Fexpr_dynenv(f),
                  Expression_new(ENVIRONMENT, *env));

  return Eval_eval(self, Fexpr_body(f), &environment);
}

static Expression *
Eval_applyStruct (Eval *self, Struct *type, char *name, Expression *args,
                  Environment **env)
{
  Expression *expr = NULL;
  Object *obj = NULL;
  int size, length;

  obj = Object_new(type, name);

  size = Struct_size(Object_type(obj));
  length = Expression_length(args);

  if (size != length) {
    Utils_error("struct %s object creation: expected %d arguments, given %d",
                Object_name(obj), size, length);
    return NULL;
  }

  while (Expression_type(args) != NIL) {
    if (Expression_type(args) != PAIR) {
      Utils_error("struct %s object creation: expected proper list",
                  Object_name(obj));
      return NULL;
    }

    if ((expr = Eval_eval(self, Expression_car(args), env)) == NULL)
      return NULL;

    Object_setFieldByPosition(obj, size - (length--), expr);

    args = Expression_cdr(args);
  }

  return Expression_new(OBJECT, obj);
}

static Expression *
Eval_applyObject (Eval *self, Object *obj, Expression *arg, Environment **env)
{
  Expression *expr = NULL;
  Symbol *field = NULL;

  if ((expr = Eval_eval(self, arg, env)) == NULL)
    return NULL;

  if (Expression_type(expr) != SYMBOL) {
    Utils_error("struct %s field access: expected symbol as argument",
                Object_name(obj));
    return NULL;
  }

  field = Expression_expr(expr);
  if ((expr = Object_field(obj, field)) == NULL) {
    Utils_error("struct %s has no field named %s",
                Object_name(obj), Symbol_name(field));
    return NULL;
  }

  return expr;
}

static Expression *
Eval_evalPair (Eval *self, Pair *pair, Environment **env)
{
  Expression *expr = NULL;

  if ((expr = Eval_eval(self, Pair_fst(pair), env)) == NULL)
    return NULL;

  switch (Expression_type(expr)) {
  case PRIMITIVE:
    return Primitive_proc(Expression_expr(expr))(Pair_snd(pair), env, self);
  case FEXPR:
    return Eval_applyFexpr(self, Expression_expr(expr), Pair_snd(pair), env);
  case STRUCT:
    return Eval_applyStruct(self, Expression_expr(expr),
                            ((Expression_type(Pair_fst(pair)) == SYMBOL)
                             ? Symbol_name(Expression_expr(Pair_fst(pair)))
                             : "%anon%"),
                            Pair_snd(pair), env);
  case OBJECT:
    return Eval_applyObject(self, Expression_expr(expr), Pair_snd(pair), env);
  default:
    Utils_error("expected callable, given a %s",
                Expression_typeName(expr));
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
  case OBJECT:
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
