#include <stdlib.h>
#include "expression.h"
#include "symbol.h"
#include "environment.h"

Environment *
Environment_new (Environment *base)
{
  if (base == NULL)
    return (Environment *) Expression_new(PAIR, NULL);
  return base;
}

void
Environment_delete (Environment *self)
{
  if (self == NULL) return;
  Expression_delete((Expression *) self);
}

/* ----- */

#define caar(e) Expression_expr(Expression_car(Expression_car(e)))
#define cdar(e) Expression_cdr(Expression_car(e))

Expression *
Environment_find (Environment *self, Symbol *sym)
{
  Expression *env = (Expression *) self;

  while (!Expression_isNil(env)) {
    if (caar(env) == sym)
      return cdar(env);
    env = Expression_cdr(env);
  }

  return NULL;
}

#undef caar
#undef cdar

Environment *
Environment_add (Environment *self, Symbol *sym, Expression *expr)
{
  return (Environment *)
    Expression_cons(Expression_cons(Expression_new(SYMBOL, sym), expr), self);
}
