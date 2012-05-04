#ifndef ENVIRONMENT_H__
#define ENVIRONMENT_H__

#include "expression.h"
#include "symbol.h"

typedef struct Environment Environment;

Environment *Environment_new (Environment *base);
void Environment_delete (Environment *self);
Environment *Environment_copy (Environment *self);

Expression *Environment_find (Environment *self, Symbol *sym);
Symbol *Environment_reverseFind (Environment *self, Expression *expr);
void Environment_add (Environment *self, Symbol *sym, Expression *expr);
void Environment_pop (Environment *self);

#endif /* ENVIRONMENT_H__ */
