#include <stdlib.h>
#include <stdio.h>
#include "utils.h"
#include "port.h"
#include "expression.h"
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

#define minmax_nb_args(proc,min,max,args,n)                             \
  if ((*n = Expression_length(args)) < min || *n > max) {               \
    Utils_error("%s: expected between %d and %d arguments, given %d",   \
                proc, min, max, *n);                                    \
    return NULL;                                                        \
  }

#define car(e)  (Expression_car(e))
#define cdr(e)  (Expression_cdr(e))
#define cadr(e) (Expression_car(Expression_cdr(e)))
#define caddr(e) (Expression_car(Expression_cdr(Expression_cdr(e))))

static Expression *
PrimitiveProc_define (Expression *args, Environment **env, Eval *ev)
{
  Expression *expr = NULL, *tmp = NULL;

  nb_args("define", 2, args);
  if (Expression_type(car(args)) != SYMBOL) {
    Utils_error("define: expected symbol as first argument");
    return NULL;
  }

  tmp = Expression_new(NIL, NULL);
  Environment_add(*env, Expression_expr(car(args)), tmp);

  if ((expr = Eval_eval(ev, cadr(args), env)) == NULL)
    return NULL;

  Expression_setType(tmp, Expression_type(expr));
  Expression_setExpr(tmp, Expression_expr(expr));

  return car(args);
}

static Expression *
PrimitiveProc_sequence (Expression *args, Environment **env, Eval *ev)
{
  Expression *expr = NULL;

  while (Expression_type(args) != NIL) {
    if (Expression_type(args) != PAIR) {
      Utils_error("sequence: expected proper list");
      return NULL;
    }
    if ((expr = Eval_eval(ev, car(args), env)) == NULL)
      return NULL;

    args = cdr(args);
  }

  return expr;
}

static Expression *
PrimitiveProc_if (Expression *args, Environment **env, Eval *ev)
{
  Expression *cond = NULL;

  nb_args("if", 3, args);

  if ((cond = Eval_eval(ev, car(args), env)) == NULL)
    return NULL;

  if (Expression_type(cond) != NIL)
    return Eval_eval(ev, cadr(args), env);
  return Eval_eval(ev, caddr(args), env);
}

static Expression *
PrimitiveProc_same (Expression *args, Environment **env, Eval *ev)
{
  Expression *expr1 = NULL, *expr2;

  nb_args("same?", 2, args);

  if ((expr1 = Eval_eval(ev, car(args), env)) == NULL)
    return NULL;

  if ((expr2 = Eval_eval(ev, cadr(args), env)) == NULL)
    return NULL;

  if (Expression_expr(expr1) == Expression_expr(expr2))
    return Expression_new(SYMBOL, Primitive_true());
  return Expression_new(NIL, NULL);
}

static Expression *
PrimitiveProc_eval (Expression *args, Environment **env, Eval *ev)
{
  Expression *expr = NULL, *environment = NULL;
  Environment *environ = NULL;
  int nb_args;

  minmax_nb_args("eval", 1, 2, args, &nb_args);

  if ((expr = Eval_eval(ev, car(args), env)) == NULL)
    return NULL;

  if (nb_args == 2) {
    if ((environment = Eval_eval(ev, cadr(args), env)) == NULL)
      return NULL;

    environ = Expression_expr(environment);
    return Eval_eval(ev, expr, &environ);
  }

  return Eval_eval(ev, expr, env);
}

static Expression *
PrimitiveProc_vau (Expression *args, Environment **env, Eval *ev)
{
  nb_args("vau", 3, args);

  if (Expression_type(car(args)) != SYMBOL ||
      Expression_type(cadr(args)) != SYMBOL ||
      Expression_expr(car(args)) == Expression_expr(cadr(args))) {
    Utils_error("vau: expected the first two arguments to be different"
                " symbols");
    return NULL;
  }

  return Expression_new(FEXPR, Fexpr_new(Expression_expr(car(args)),
                                         Expression_expr(cadr(args)),
                                         caddr(args),
                                         Environment_copy(*env)));
}

static Expression *
PrimitiveProc_environment (Expression *args, Environment **env, Eval *ev)
{
  nb_args("environment", 0, args);

  return Expression_new(ENVIRONMENT, Environment_copy(*env));
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

  if (Expression_type(expr) != PAIR) {
    Utils_error("car: expected pair");
    return NULL;
  }

  return car(expr);
}

static Expression *
PrimitiveProc_cdr (Expression *args, Environment **env, Eval *ev)
{
  Expression *expr = NULL;

  nb_args("cdr", 1, args);

  if ((expr = Eval_eval(ev, car(args), env)) == NULL)
    return NULL;

  if (Expression_type(expr) != PAIR) {
    Utils_error("cdr: expected pair");
    return NULL;
  }

  return cdr(expr);
}

static Expression *
PrimitiveProc_list (Expression *args, Environment **env, Eval *ev)
{
  Expression *list = NULL, *expr = NULL, *tmp = NULL, *prev = NULL;

  list = tmp = Expression_new(PAIR, Pair_new(NULL, NULL));
  while (Expression_type(args) == PAIR) {
    if ((expr = Eval_eval(ev, car(args), env)) == NULL)
      return NULL;

    Expression_setCar(tmp, expr);
    Expression_setCdr(tmp, Expression_new(PAIR, Pair_new(NULL, NULL)));
    prev = tmp;
    tmp = cdr(tmp);
    args = cdr(args);
  }

  if (Expression_type(args) != NIL) {
    if ((expr = Eval_eval(ev, args, env)) == NULL)
      return NULL;

    Expression_setCdr(prev, expr);
  }
  else {
    Expression_setExpr(tmp, NULL);
    Expression_setType(tmp, NIL);
  }


  return list;
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
PrimitiveProc_struct (Expression *args, Environment **env, Eval *ev)
{
  Struct *structure = NULL;
  Expression *member = NULL;
  int i = 0, size;

  size = Expression_length(args);

  structure = Struct_new(size);

  while (Expression_type(args) != NIL) {
    if (Expression_type(args) != PAIR) {
      Utils_error("struct: expected proper list");
      return NULL;
    }

    if (Expression_type(member = car(args)) != SYMBOL) {
      Utils_error("struct: members name must be symbol");
      return NULL;
    }

    Struct_declareMember(structure, i++, Expression_expr(member));

    args = cdr(args);
  }

  return Expression_new(STRUCT, structure);
}

static Expression *
PrimitiveProc_type (Expression *args, Environment **env, Eval *ev)
{
  Expression *expr = NULL;

  nb_args("type", 1, args);

  if ((expr = Eval_eval(ev, car(args), env)) == NULL)
    return NULL;

  if (Expression_type(expr) != OBJECT) {
    Utils_error("type: expected object has argument");
    return NULL;
  }

  return Expression_new(STRUCT, Object_type(Expression_expr(expr)));
}

/* --- */

static Expression *
PrimitiveHelper_typep (char *name, ExprType type,
                       Expression *args, Environment **env, Eval *ev)
{
  Expression *expr = NULL;

  nb_args(name, 1, args);

  if ((expr = Eval_eval(ev, car(args), env)) == NULL)
    return NULL;

  if (Expression_type(expr) == type)
    return Expression_new(SYMBOL, Primitive_true());
  return Expression_new(NIL, NULL);
}

static Expression *
PrimitiveProc_primitivep (Expression *args, Environment **env, Eval *ev)
{
  return PrimitiveHelper_typep("primitive?", PRIMITIVE, args, env, ev);
}

static Expression *
PrimitiveProc_nullp (Expression *args, Environment **env, Eval *ev)
{
  return PrimitiveHelper_typep("null?", NIL, args, env, ev);
}

static Expression *
PrimitiveProc_pairp (Expression *args, Environment **env, Eval *ev)
{
  return PrimitiveHelper_typep("pair?", PAIR, args, env, ev);
}

static Expression *
PrimitiveProc_symbolp (Expression *args, Environment **env, Eval *ev)
{
  return PrimitiveHelper_typep("symbol?", SYMBOL, args, env, ev);
}

static Expression *
PrimitiveProc_characterp (Expression *args, Environment **env, Eval *ev)
{
  return PrimitiveHelper_typep("character?", CHARACTER, args, env, ev);
}

static Expression *
PrimitiveProc_stringp (Expression *args, Environment **env, Eval *ev)
{
  return PrimitiveHelper_typep("string?", STRING, args, env, ev);
}

static Expression *
PrimitiveProc_numberp (Expression *args, Environment **env, Eval *ev)
{
  return PrimitiveHelper_typep("number?", NUMBER, args, env, ev);
}

static Expression *
PrimitiveProc_fexprp (Expression *args, Environment **env, Eval *ev)
{
  return PrimitiveHelper_typep("fexpr?", FEXPR, args, env, ev);
}

static Expression *
PrimitiveProc_environmentp (Expression *args, Environment **env, Eval *ev)
{
  return PrimitiveHelper_typep("environment?", ENVIRONMENT, args, env, ev);
}

static Expression *
PrimitiveProc_structp (Expression *args, Environment **env, Eval *ev)
{
  return PrimitiveHelper_typep("struct?", STRUCT, args, env, ev);
}

static Expression *
PrimitiveProc_objectp (Expression *args, Environment **env, Eval *ev)
{
  return PrimitiveHelper_typep("object?", OBJECT, args, env, ev);
}

/* --- */

static Expression *
PrimitiveProc_error (Expression *args, Environment **env, Eval *ev)
{
  Expression *expr = NULL;

  nb_args("error", 1, args);

  if ((expr = Eval_eval(ev, car(args), env)) == NULL)
    return NULL;

  if (Expression_type(expr) != STRING) {
    Utils_error("error: expected string");
    return NULL;
  }

  Port_printf(Eval_errput(ev), "Error: %s\n",
              String_buf(Expression_expr(expr)));

  return NULL;
}

/* --- */

static Expression *
PrimitiveHelper_arith (char *name, Number *(*op)(Number *, Number*),
                       Expression *args, Environment **env, Eval *ev,
                       Number *neutral)
{
  Expression *list = args, *tmp = NULL;
  Number *result = neutral;

  while (Expression_type(list) != NIL) {
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

static bool
PrimitiveHelper_arithp (char *name, bool (*pred)(Number *, Number*),
                        Expression *args, Environment **env, Eval *ev)
{
  Expression *expr = NULL, *prev = NULL;

  min_nb_args(name, 2, args);

  if ((prev = Eval_eval(ev, car(args), env)) == NULL)
    return NULL;

  if (Expression_type(prev) != NUMBER) {
    Utils_error("%s: expected number", name);
    return NULL;
  }

  args = cdr(args);

  while (Expression_type(args) != NIL) {
    if (Expression_type(args) != PAIR) {
      Utils_error("%s: expected proper argument list", name);
      return NULL;
    }

    if ((expr = Eval_eval(ev, car(args), env)) == NULL)
      return NULL;

    if (Expression_type(expr) != NUMBER) {
      Utils_error("%s: expected number", name);
      return NULL;
    }

    if (!pred(Expression_expr(prev), Expression_expr(expr)))
      return false;

    prev = expr;
    args = cdr(args);
  }

  return true;
}

static Expression *
PrimitiveProc_equal (Expression *args, Environment **env, Eval *ev)
{
  if (PrimitiveHelper_arithp("=", Number_eq, args, env, ev))
    return Expression_new(SYMBOL, Primitive_true());
  return Expression_new(NIL, NULL);
}

static Expression *
PrimitiveProc_lesser (Expression *args, Environment **env, Eval *ev)
{
  if (PrimitiveHelper_arithp("<", Number_lt, args, env, ev))
    return Expression_new(SYMBOL, Primitive_true());
  return Expression_new(NIL, NULL);
}

static Expression *
PrimitiveProc_greater (Expression *args, Environment **env, Eval *ev)
{
  if (PrimitiveHelper_arithp(">", Number_gt, args, env, ev))
    return Expression_new(SYMBOL, Primitive_true());
  return Expression_new(NIL, NULL);
}

/* --- */

static Expression *
PrimitiveProc_open_fexpr (Expression *args, Environment **env, Eval *ev)
{
  Expression *expr = NULL;

  nb_args("%open-fexpr%", 1, args);

  if ((expr = Eval_eval(ev, car(args), env)) == NULL)
    return NULL;

  if (Expression_type(expr) != FEXPR) {
    Utils_error("%%open-fexpr%%: expected fexpr");
    return NULL;
  }

  return Expression_cons(Fexpr_body(Expression_expr(expr)),
                         Expression_new(ENVIRONMENT,
                                        Fexpr_lexenv(Expression_expr(expr))));
}

static Expression *
PrimitiveProc_open_struct (Expression *args, Environment **env, Eval *ev)
{
  Expression *expr = NULL;
  Symbol **members = NULL;
  int i, size;

  nb_args("%open-struct%", 1, args);

  if ((expr = Eval_eval(ev, car(args), env)) == NULL)
    return NULL;

  if (Expression_type(expr) != STRUCT) {
    Utils_error("%%open-struct%%: expected struct");
    return NULL;
  }

  size = Struct_size(Expression_expr(expr));
  members = Struct_members(Expression_expr(expr));

  expr = Expression_new(NIL, NULL);
  for (i = size - 1; i >= 0; i--)
    expr = Expression_cons(Expression_new(SYMBOL, members[i]), expr);

  return expr;
}

#undef car
#undef cdr
#undef cadr
#undef caddr

#undef nb_args

/* ----- */

#define PRIMITIVE_COUNT 37

Primitive prim_[PRIMITIVE_COUNT] = {
  { "define",       PrimitiveProc_define },
  { "sequence",     PrimitiveProc_sequence },
  { "if",           PrimitiveProc_if },
  { "same?",        PrimitiveProc_same },
  { "eval",         PrimitiveProc_eval },
  { "vau",          PrimitiveProc_vau },
  { "environment",  PrimitiveProc_environment },

  { "cons",         PrimitiveProc_cons },
  { "car",          PrimitiveProc_car },
  { "cdr",          PrimitiveProc_cdr },
  { "list",         PrimitiveProc_list },
  { "length",       PrimitiveProc_length },

  { "struct",       PrimitiveProc_struct },
  { "type",         PrimitiveProc_type },

  { "null?",        PrimitiveProc_nullp },
  { "primitive?",   PrimitiveProc_primitivep },
  { "pair?",        PrimitiveProc_pairp },
  { "symbol?",      PrimitiveProc_symbolp },
  { "character?",   PrimitiveProc_characterp },
  { "string?",      PrimitiveProc_stringp },
  { "number?",      PrimitiveProc_numberp },
  { "fexpr?",       PrimitiveProc_fexprp },
  { "environment?", PrimitiveProc_environmentp },
  { "struct?",      PrimitiveProc_structp },
  { "object?",      PrimitiveProc_objectp },

  { "error",        PrimitiveProc_error },

  { "+",            PrimitiveProc_add },
  { "-",            PrimitiveProc_sub },
  { "*",            PrimitiveProc_mul },
  { "/",            PrimitiveProc_div },
  { "div",          PrimitiveProc_idiv },
  { "mod",          PrimitiveProc_mod },
  { "=",            PrimitiveProc_equal },
  { "<",            PrimitiveProc_lesser },
  { ">",            PrimitiveProc_greater },

  { "%open-fexpr%", PrimitiveProc_open_fexpr },
  { "%open-struct%", PrimitiveProc_open_struct }
};

Symbol *
Primitive_true ()
{
  static Symbol *t = NULL;

  if (t == NULL)
    t = Symbol_new("t");

  return t;
}

Expression *
Primitive_initialSymbols ()
{
  static Expression *symbols = NULL;
  int i;

  if (symbols != NULL)
    return symbols;

  symbols = Expression_new(NIL, NULL);
  for (i = 0; i < PRIMITIVE_COUNT; i++) {
    symbols = Expression_cons(Expression_new(SYMBOL,
                                             Symbol_new(prim_[i].name)),
                              symbols);
  }
  symbols = Expression_cons(Expression_new(SYMBOL, Primitive_true()),
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
    Environment_add(env, Utils_findSymbol(symbols, prim_[i].name),
                    Expression_new(PRIMITIVE, &prim_[i]));
  }
  t = Primitive_true();
  Environment_add(env, t, Expression_new(SYMBOL, t));

  return env;
}
