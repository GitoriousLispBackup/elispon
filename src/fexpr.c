#include <stdlib.h>
#include "utils.h"
#include "expression.h"
#include "environment.h"
#include "fexpr.h"

struct Fexpr {
  Symbol *arg;
  Symbol *dynenv;
  Expression *body;
  Environment *lexenv;
};

Fexpr *
Fexpr_new (Symbol *arg, Symbol *dynenv,
           Expression *body, Environment *lexenv)
{
  Fexpr *self = NULL;

  alloc_one(self);

  self->arg = arg;
  self->dynenv = dynenv;
  self->body = body;
  self->lexenv = lexenv;

  return self;
}

void
Fexpr_delete (Fexpr *self)
{
  if (self == NULL) return;
  free_(self);
}

/* ----- */

Symbol *
Fexpr_arg (Fexpr *self)
{
  return self->arg;
}

Symbol *
Fexpr_dynenv (Fexpr *self)
{
  return self->dynenv;
}

Expression *
Fexpr_body (Fexpr *self)
{
  return self->body;
}

Expression *
Fexpr_lexenv (Fexpr *self)
{
  return self->lexenv;
}
