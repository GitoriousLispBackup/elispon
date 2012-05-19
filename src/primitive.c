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
#include "vector.h"
#include "environment.h"
#include "struct.h"
#include "object.h"
#include "eval.h"
#include "printer.h"
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
#define cons(a,b) (Expression_cons(a, b))

#include "primitives/core.c"
#include "primitives/list.c"
#include "primitives/vector.c"
#include "primitives/struct.c"
#include "primitives/predicates.c"
#include "primitives/io.c"
#include "primitives/math.c"
#include "primitives/conversion.c"
#include "primitives/introspection.c"

#undef car
#undef cdr
#undef cadr
#undef caddr
#undef cons

#undef nb_args

/* ----- */

Primitive prim_[PRIMITIVE_COUNT] = {
  { "define",         PrimitiveProc_define },
  { "set",            PrimitiveProc_set },
  { "sequence",       PrimitiveProc_sequence },
  { "if",             PrimitiveProc_if },
  { "same?",          PrimitiveProc_same },
  { "eval",           PrimitiveProc_eval },
  { "vau",            PrimitiveProc_vau },
  { "environment",    PrimitiveProc_environment },
  { "length",         PrimitiveProc_length },

  { "cons",           PrimitiveProc_cons },
  { "car",            PrimitiveProc_car },
  { "cdr",            PrimitiveProc_cdr },
  { "list",           PrimitiveProc_list },

  { "vector",         PrimitiveProc_vector },

  { "struct",         PrimitiveProc_struct },
  { "type",           PrimitiveProc_type },

  { "null?",          PrimitiveProc_nullp },
  { "primitive?",     PrimitiveProc_primitivep },
  { "pair?",          PrimitiveProc_pairp },
  { "symbol?",        PrimitiveProc_symbolp },
  { "character?",     PrimitiveProc_characterp },
  { "string?",        PrimitiveProc_stringp },
  { "number?",        PrimitiveProc_numberp },
  { "fexpr?",         PrimitiveProc_fexprp },
  { "vector?",        PrimitiveProc_vectorp },
  { "environment?",   PrimitiveProc_environmentp },
  { "struct?",        PrimitiveProc_structp },
  { "object?",        PrimitiveProc_objectp },

  { "write",          PrimitiveProc_write },
  { "print",          PrimitiveProc_print },
  { "error",          PrimitiveProc_error },
  { "read",           PrimitiveProc_read },
  { "scan",           PrimitiveProc_scan },

  { "+",              PrimitiveProc_add },
  { "-",              PrimitiveProc_sub },
  { "*",              PrimitiveProc_mul },
  { "/",              PrimitiveProc_div },
  { "div",            PrimitiveProc_idiv },
  { "mod",            PrimitiveProc_mod },
  { "round",          PrimitiveProc_round },
  { "abs",            PrimitiveProc_abs },
  { "=",              PrimitiveProc_equal },
  { "<",              PrimitiveProc_lesser },
  { ">",              PrimitiveProc_greater },

  { "symbol->string", PrimitiveProc_symbol_to_string },
  { "string->symbol", PrimitiveProc_string_to_symbol },
  { "number->string", PrimitiveProc_number_to_string },
  { "string->number", PrimitiveProc_string_to_number },
  { "string->list",   PrimitiveProc_string_to_list },
  { "vector->list",   PrimitiveProc_vector_to_list },
  { "list->vector",   PrimitiveProc_list_to_vector },

  { "%open-fexpr%",   PrimitiveProc_open_fexpr },
  { "%open-struct%",  PrimitiveProc_open_struct }
};

Symbol *
Primitive_true ()
{
  static Symbol *t = NULL;

  if (t == NULL)
    t = Symbol_new("t");

  return t;
}

char **
Primitive_initialSymbols ()
{
  static char **symbols = NULL;
  int i;

  if (symbols != NULL)
    return symbols;

  alloc_(symbols, PRIMITIVE_COUNT);

  for (i = 0; i < PRIMITIVE_COUNT; i++)
    symbols[i] = prim_[i].name;

  return symbols;
}

Environment *
Primitive_initialEnvironment (SymbolTable *symbols)
{
  static Environment *env = NULL;
  Symbol *t = NULL;
  int i;

  if (env != NULL)
    return env;

  env = Environment_new(NULL);
  for (i = 0; i < PRIMITIVE_COUNT; i++)
    Environment_add(env, SymbolTable_find(symbols, prim_[i].name),
                    Expression_new(PRIMITIVE, &prim_[i]));
  t = Primitive_true();
  Environment_add(env, t, Expression_new(SYMBOL, t));

  return env;
}
