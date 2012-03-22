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

PrimitiveProc
Primitive_proc (Primitive *self)
{
  return self->proc;
}

/* ----- */

#define car(e) Expression_car(e)
#define cdr(e) Expression_cdr(e)
#define cadr(e) Expression_car(Expression_cdr(e))
#define cddr(e) Expression_cdr(Expression_cdr(e))

Expression *
PrimitiveProc_define (Expression *args, Environment **env)
{
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
  *env = Environment_add(*env, Expression_expr(car(args)), cadr(args));
  return car(args);
}

#undef car
#undef cadr

/* ----- */

#define PRIMITIVE_COUNT 1

Primitive prim_[PRIMITIVE_COUNT] = {
  { "define", PrimitiveProc_define }
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

