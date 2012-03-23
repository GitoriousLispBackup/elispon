#include <stdlib.h>
#include <stdio.h>
#include "utils.h"
#include "expression.h"
#include "symbol.h"
#include "string.h"
#include "number.h"
#include "fexpr.h"
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

#define nb_args(proc,n,args)                                        \
  if (Expression_length(args) != n) {                               \
    Utils_error("%s: expected %d argument%s, given %d",             \
                proc, n, ((n==1)?"":"s"), Expression_length(args)); \
    return NULL;                                                    \
  }

#define min_nb_args(proc,n,args)                                        \
  if (Expression_length(args) < n) {                                    \
    Utils_error("%s: expected at least %d argument%s, given %d",        \
                proc, n, ((n==1)?"":"s"), Expression_length(args));     \
    return NULL;                                                        \
  }

#define car(e)  (Expression_car(e))
#define cdr(e)  (Expression_cdr(e))
#define cadr(e) (Expression_car(Expression_cdr(e)))
#define caddr(e) (Expression_car(Expression_cdr(Expression_cdr(e))))

static Expression *
PrimitiveProc_define (Expression *args, Environment **env, Eval *ev)
{
  Expression *expr = NULL;

  nb_args("define", 2, args);
  if (Expression_type(car(args)) != SYMBOL) {
    Utils_error("define: expected symbol as first argument");
    return NULL;
  }

  if ((expr = Eval_eval(ev, cadr(args), env)) == NULL)
    return NULL;

  *env = Environment_add(*env, Expression_expr(car(args)), expr);

  return car(args);
}

/* --- */

static Expression *
PrimitiveProc_cons (Expression *args, Environment **env, Eval *ev)
{
  Expression *fst = NULL, *snd = NULL;

  nb_args("cons", 2, args);

  if ((fst = Eval_eval(ev, car(args), env)) == NULL)
    return NULL;
  if ((snd = Eval_eval(ev, cadr(args), env)) == NULL)
    return NULL;

  return Expression_cons(fst, snd);
}

static Expression *
PrimitiveProc_car (Expression *args, Environment **env, Eval *ev)
{
  Expression *expr = NULL;

  nb_args("car", 1, args);

  if ((expr = Eval_eval(ev, car(args), env)) == NULL)
    return NULL;

  return Expression_car(expr);
}

static Expression *
PrimitiveProc_cdr (Expression *args, Environment **env, Eval *ev)
{
  Expression *expr = NULL;

  nb_args("cdr", 1, args);

  if ((expr = Eval_eval(ev, car(args), env)) == NULL)
    return NULL;

  return Expression_cdr(expr);
}

static Expression *
PrimitiveProc_list (Expression *args, Environment **env, Eval *ev)
{
  return Eval_mapEval(ev, args, env);
}

static Expression *
PrimitiveProc_length (Expression *args, Environment **env, Eval *ev)
{
  Expression *list = NULL;

  nb_args("length", 1, args);

  if ((list = Eval_eval(ev, car(args), env)) == NULL)
    return NULL;

  return Expression_new(NUMBER, Number_new(Expression_length(list)));
}

/* --- */

static Expression *
PrimitiveHelper_arith (char *name, Number *(*op)(Number *, Number*),
                       Expression *args, Environment **env, Eval *ev,
                       Number *neutral)
{
  Expression *list = args, *tmp = NULL;
  Number *result = neutral;

  while (!Expression_isNil(list)) {
    if (Expression_type(list) != PAIR) {
      Utils_error("%s: expected proper argument list", name);
      return NULL;
    }
    if ((tmp = Eval_eval(ev, car(list), env)) == NULL)
      return NULL;
    if (Expression_type(tmp) != NUMBER) {
      Utils_error("%s: expected number", name);
      return NULL;
    }
    result = op(result, Expression_expr(tmp));
    list = cdr(list);
  }

  return Expression_new(NUMBER, result);
}

static Expression *
PrimitiveHelper_arith1 (char *name, Number *(*op)(Number *, Number*),
                        Expression *args, Environment **env, Eval *ev)
{
  Expression *expr = NULL;

  min_nb_args(name, 1, args);

  if (Expression_length(args) == 1)
    return PrimitiveHelper_arith(name, op, args, env, ev, Number_new(1));

  if ((expr = Eval_eval(ev, car(args), env)) == NULL)
    return NULL;
  if (Expression_type(expr) != NUMBER) {
    Utils_error("%s: expected number", name);
    return NULL;
  }

  return PrimitiveHelper_arith(name, op, cdr(args), env, ev,
                               Expression_expr(expr));
}

static Expression *
PrimitiveProc_add (Expression *args, Environment **env, Eval *ev)
{
  return PrimitiveHelper_arith("+", Number_add, args, env, ev, Number_new(0));
}

static Expression *
PrimitiveProc_sub (Expression *args, Environment **env, Eval *ev)
{
  return PrimitiveHelper_arith("-", Number_sub, args, env, ev, Number_new(0));
}

static Expression *
PrimitiveProc_mul (Expression *args, Environment **env, Eval *ev)
{
  return PrimitiveHelper_arith("*", Number_mul, args, env, ev, Number_new(1));
}

static Expression *
PrimitiveProc_div (Expression *args, Environment **env, Eval *ev)
{
  return PrimitiveHelper_arith1("/", Number_div, args, env, ev);
}

static Expression *
PrimitiveProc_idiv (Expression *args, Environment **env, Eval *ev)
{
  return PrimitiveHelper_arith1("div", Number_idiv, args, env, ev);
}

static Expression *
PrimitiveProc_mod (Expression *args, Environment **env, Eval *ev)
{
  return PrimitiveHelper_arith1("mod", Number_mod, args, env, ev);
}

static Expression *
PrimitiveProc_equal (Expression *args, Environment **env, Eval *ev)
{
  Expression *symbols = Primitive_initialSymbols(), *expr = NULL;

  if ((expr = PrimitiveHelper_arith1("=", Number_eq, args, env, ev)) == NULL)
    return NULL;

  if (Number_val(Expression_expr(expr)))
    return Expression_new(SYMBOL, Utils_findSymbol(symbols, "t"));
  return Expression_new(PAIR, NULL);
}

/* --- */

static Expression *
PrimitiveProc_if (Expression *args, Environment **env, Eval *ev)
{
  Expression *cond = NULL;

  nb_args("if", 3, args);

  if ((cond = Eval_eval(ev, car(args), env)) == NULL)
    return NULL;

  if (!Expression_isNil(cond))
    return Eval_eval(ev, cadr(args), env);
  return Eval_eval(ev, caddr(args), env);
}

/* --- */

static Expression *
PrimitiveProc_nullp (Expression *args, Environment **env, Eval *ev)
{
  Expression *symbols = Primitive_initialSymbols(), *expr = NULL;

  nb_args("null?", 1, args);

  if ((expr = Eval_eval(ev, car(args), env)) == NULL)
    return NULL;

  if (Expression_isNil(expr))
    return Expression_new(SYMBOL, Utils_findSymbol(symbols, "t"));
  return Expression_new(PAIR, NULL);
}

static Expression *
PrimitiveHelper_typep (char *name, ExprType type,
                       Expression *args, Environment **env, Eval *ev)
{
  Expression *symbols = Primitive_initialSymbols(), *expr = NULL;

  nb_args(name, 1, args);

  if ((expr = Eval_eval(ev, car(args), env)) == NULL)
    return NULL;

  if (Expression_type(expr) == type)
    return Expression_new(SYMBOL, Utils_findSymbol(symbols, "t"));
  return Expression_new(PAIR, NULL);
}

static Expression *
PrimitiveProc_primitivep (Expression *args, Environment **env, Eval *ev)
{
  return PrimitiveHelper_typep("primitive?", PRIMITIVE, args, env, ev);
}

static Expression *
PrimitiveProc_pairp (Expression *args, Environment **env, Eval *ev)
{
  Expression *symbols = Primitive_initialSymbols(), *expr = NULL;

  nb_args("pair?", 1, args);

  if ((expr = Eval_eval(ev, car(args), env)) == NULL)
    return NULL;

  if (Expression_type(expr) == PAIR && !Expression_isNil(expr))
    return Expression_new(SYMBOL, Utils_findSymbol(symbols, "t"));
  return Expression_new(PAIR, NULL);
}

static Expression *
PrimitiveProc_symbolp (Expression *args, Environment **env, Eval *ev)
{
  return PrimitiveHelper_typep("symbol?", SYMBOL, args, env, ev);
}

static Expression *
PrimitiveProc_numberp (Expression *args, Environment **env, Eval *ev)
{
  return PrimitiveHelper_typep("number?", NUMBER, args, env, ev);
}

static Expression *
PrimitiveProc_stringp (Expression *args, Environment **env, Eval *ev)
{
  return PrimitiveHelper_typep("string?", STRING, args, env, ev);
}

static Expression *
PrimitiveProc_fexprp (Expression *args, Environment **env, Eval *ev)
{
  return PrimitiveHelper_typep("fexpr?", FEXPR, args, env, ev);
}

/* --- */

static Expression *
PrimitiveProc_epsilon (Expression *args, Environment **env, Eval *ev)
{
  min_nb_args("ε", 3, args);

  if (Expression_type(car(args)) != SYMBOL ||
      Expression_type(cadr(args)) != SYMBOL ||
      Expression_expr(car(args)) == Expression_expr(cadr(args))) {
    Utils_error("ε: expected the first two arguments to be different symbols");
    return NULL;
  }

  return Expression_new(FEXPR, Fexpr_new(Expression_expr(car(args)),
                                         Expression_expr(cadr(args)),
                                         caddr(args), *env));
}

/* --- */

static Expression *
PrimitiveProc_environment (Expression *args, Environment **env, Eval *ev)
{
  nb_args("environment", 0, args);

  return *env;
}

static Expression *
PrimitiveProc_eval (Expression *args, Environment **env, Eval *ev)
{
  Expression *expr = NULL;
  Environment *environment = NULL;

  nb_args("eval", 2, args);

  if ((environment = Eval_eval(ev, cadr(args), env)) == NULL)
    return NULL;
  if ((expr = Eval_eval(ev, car(args), env)) == NULL)
    return NULL;

  return Eval_eval(ev, expr, &environment);
}

static Expression *
PrimitiveProc_apply (Expression *args, Environment **env, Eval *ev)
{
  Expression *expr = NULL;

  nb_args("apply", 2, args);

  if ((expr = Eval_eval(ev, cadr(args), env)) == NULL)
    return NULL;

  Expression_setCdr(args, expr);

  return Eval_eval(ev, args, env);
}

#undef car
#undef cdr
#undef cadr
#undef caddr

#undef nb_args

/* ----- */

#define PRIMITIVE_COUNT 25

Primitive prim_[PRIMITIVE_COUNT] = {
  { "define",      PrimitiveProc_define },

  { "cons",        PrimitiveProc_cons },
  { "car",         PrimitiveProc_car },
  { "cdr",         PrimitiveProc_cdr },
  { "list",        PrimitiveProc_list },
  { "length",      PrimitiveProc_length },

  { "+",           PrimitiveProc_add },
  { "-",           PrimitiveProc_sub },
  { "*",           PrimitiveProc_mul },
  { "/",           PrimitiveProc_div },
  { "div",         PrimitiveProc_idiv },
  { "mod",         PrimitiveProc_mod },
  { "=",           PrimitiveProc_equal },

  { "if",          PrimitiveProc_if },

  { "null?",       PrimitiveProc_nullp },
  { "primitive?",  PrimitiveProc_primitivep },
  { "pair?",       PrimitiveProc_pairp },
  { "symbol?",     PrimitiveProc_symbolp },
  { "string?",     PrimitiveProc_stringp },
  { "number?",     PrimitiveProc_numberp },
  { "fexpr?",      PrimitiveProc_fexprp },

  { "ε",           PrimitiveProc_epsilon },

  { "environment", PrimitiveProc_environment },
  { "eval",        PrimitiveProc_eval },
  { "apply",       PrimitiveProc_apply }
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
  symbols = Expression_cons(Expression_new(PAIR, Symbol_new("t")),
                            symbols);

  return symbols;
}

Environment *
Primitive_initialEnvironment ()
{
  static Environment *env = NULL;
  Expression *symbols = NULL;
  Symbol *t = NULL;
  int i;

  if (env != NULL)
    return env;

  env = Environment_new(NULL);
  symbols = Primitive_initialSymbols();
  for (i = 0; i < PRIMITIVE_COUNT; i++) {
    env = Environment_add(env, Utils_findSymbol(symbols, prim_[i].name),
                          Expression_new(PRIMITIVE, &prim_[i]));
  }
  t = Utils_findSymbol(symbols, "t");
  env = Environment_add(env, t, Expression_new(SYMBOL, t));

  return env;
}

