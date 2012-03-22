#include <stdlib.h>
#include <stdio.h>
#include "utils.h"
#include "expression.h"
#include "symbol.h"
#include "environment.h"
#include "eval.h"
#include "primitive.h"

struct Primitive {
  char *name;
  PrimitiveProc proc;
};

char *
Primitive_name (Primitive *self)
{
  return self->name;
}

PrimitiveProc
Primitive_proc (Primitive *self)
{
  return self->proc;
}

/* ----- */

#define car(e)  (Expression_car(e))
#define cdr(e)  (Expression_cdr(e))
#define cadr(e) (Expression_car(Expression_cdr(e)))
#define cddr(e) (Expression_cdr(Expression_cdr(e)))

Expression *
PrimitiveProc_define (Expression *args, Environment **env, Eval *ev)
{
  Expression *expr = NULL;

  if (Expression_isNil(args) || Expression_type(args) != PAIR ||
      Expression_type(cdr(args)) != PAIR || Expression_isNil(cdr(args)) ||
      !Expression_isNil(cddr(args))) {
    Utils_error("define: expected two arguments");
    return NULL;
  }
  if (Expression_type(car(args)) != SYMBOL) {
    Utils_error("define: expected symbol as first argument");
    return NULL;
  }

  if ((expr = Eval_eval(ev, cadr(args), env)) == NULL)
    return NULL;

  *env = Environment_add(*env, Expression_expr(car(args)), expr);

  return car(args);
}

Expression *
PrimitiveProc_cons (Expression *args, Environment **env, Eval *ev)
{
  Expression *fst = NULL, *snd = NULL;

  if (Expression_isNil(args) || Expression_type(args) != PAIR ||
      Expression_type(cdr(args)) != PAIR || Expression_isNil(cdr(args)) ||
      !Expression_isNil(cddr(args))) {
    Utils_error("cons: expected two arguments");
    return NULL;
  }

  if ((fst = Eval_eval(ev, car(args), env)) == NULL)
    return NULL;
  if ((snd = Eval_eval(ev, cadr(args), env)) == NULL)
    return NULL;

  return Expression_cons(fst, snd);
}

Expression *
PrimitiveProc_car (Expression *args, Environment **env, Eval *ev)
{
  Expression *expr = NULL;

  if (Expression_isNil(args) || Expression_type(args) != PAIR ||
      !Expression_isNil(cdr(args))) {
    Utils_error("car: expected one pair argument");
    return NULL;
  }

  if ((expr = Eval_eval(ev, car(args), env)) == NULL)
    return NULL;

  return Expression_car(expr);
}

Expression *
PrimitiveProc_cdr (Expression *args, Environment **env, Eval *ev)
{
  Expression *expr = NULL;

  if (Expression_isNil(args) || Expression_type(args) != PAIR ||
      !Expression_isNil(cdr(args))) {
    Utils_error("cdr: expected one pair argument");
    return NULL;
  }

  if ((expr = Eval_eval(ev, car(args), env)) == NULL)
    return NULL;

  return Expression_cdr(expr);
}

Expression *
PrimitiveProc_list (Expression *args, Environment **env, Eval *ev)
{
  return Eval_mapEval(ev, args, env);
}

Expression *
PrimitiveProc_environment (Expression *args, Environment **env, Eval *ev)
{
  return *env;
}

Expression *
PrimitiveProc_eval (Expression *args, Environment **env, Eval *ev)
{
  Environment *environment = NULL;

  if (Expression_isNil(args) || Expression_type(args) != PAIR ||
      Expression_type(cdr(args)) != PAIR || Expression_isNil(cdr(args)) ||
      !Expression_isNil(cddr(args))) {
    Utils_error("eval: expected two arguments");
    return NULL;
  }

  if ((environment = Eval_eval(ev, cadr(args), env)) == NULL)
    return NULL;

  return Eval_eval(ev, car(args), &environment);
}

#undef car
#undef cdr
#undef caar
#undef cadr

/* ----- */

#define PRIMITIVE_COUNT 7

Primitive prim_[PRIMITIVE_COUNT] = {
  { "define",      PrimitiveProc_define },
  { "cons",        PrimitiveProc_cons },
  { "car",         PrimitiveProc_car },
  { "cdr",         PrimitiveProc_cdr },
  { "list",        PrimitiveProc_list },
  { "environment", PrimitiveProc_environment },
  { "eval",        PrimitiveProc_eval }
};

Expression *
Primitive_initialSymbols ()
{
  static Expression *symbols = NULL;
  int i;

  if (symbols != NULL)
    return symbols;

  symbols = Expression_new(PAIR, NULL);
  for (i = 0; i < PRIMITIVE_COUNT; i++) {
    symbols = Expression_cons(Expression_new(PAIR, Symbol_new(prim_[i].name)),
                              symbols);
  }

  return symbols;
}

Environment *
Primitive_initialEnvironment ()
{
  static Environment *env = NULL;
  Expression *symbols = NULL;
  int i;

  if (env != NULL)
    return env;

  env = Environment_new();
  symbols = Primitive_initialSymbols();
  for (i = 0; i < PRIMITIVE_COUNT; i++) {
    env = Environment_add(env, Utils_findSymbol(symbols, prim_[i].name),
                          Expression_new(PRIMITIVE, &prim_[i]));
  }

  return env;
}

