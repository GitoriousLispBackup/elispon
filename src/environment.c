#include <stdlib.h>
#include "utils.h"
#include "expression.h"
#include "symbol.h"
#include "environment.h"

struct Environment {
  Expression *assoc;
  Environment *parent;
};

Environment *
Environment_new (Environment *parent)
{
  Environment *self = NULL;

  alloc_one(self);

  self->assoc = Expression_new(NIL, NULL);
  self->parent = parent;

  return self;
}

void
Environment_delete (Environment *self)
{
  if (self == NULL) return;
  Environment_delete(self->parent);
  free_(self);
}

/* ----- */

#define caar(e) Expression_expr(Expression_car(Expression_car(e)))
#define cdar(e) Expression_cdr(Expression_car(e))

Expression *
Environment_find (Environment *self, Symbol *sym)
{
  Expression *assoc = self->assoc;

  do {
    while (Expression_type(assoc) != NIL) {
      if (caar(assoc) == sym)
        return cdar(assoc);
      assoc = Expression_cdr(assoc);
    }
  } while ((self = self->parent) != NULL &&
           (assoc = self->assoc));

  return NULL;
}

Symbol *
Environment_reverseFind (Environment *self, Expression *expr)
{
  Expression *assoc = self->assoc;

  do {
    while (Expression_type(assoc) != NIL) {
      if (cdar(assoc) == expr)
        return caar(assoc);
      assoc = Expression_cdr(assoc);
    }
  } while ((self = self->parent) != NULL &&
           (assoc = self->assoc));

  return NULL;
}

void
Environment_add (Environment *self, Symbol *sym, Expression *expr)
{
  self->assoc = Expression_cons(Expression_cons(Expression_new(SYMBOL, sym),
                                                expr),
                                self->assoc);
}

void
Environment_set (Environment *self, Symbol *sym, Expression *expr)
{
  Expression *assoc = self->assoc;

  do {
    while (Expression_type(assoc) != NIL) {
      if (caar(assoc) == sym) {
        Expression_setCdr(Expression_car(assoc), expr);
        return;
      }
      assoc = Expression_cdr(assoc);
    }
  } while ((self = self->parent) != NULL &&
           (assoc = self->assoc));
}

#undef caar
#undef cdar
