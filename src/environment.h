#ifndef ENVIRONMENT_H__
#define ENVIRONMENT_H__

#include "expression.h"
#include "symbol.h"

typedef struct Environment Environment;

Environment *Environment_new (Environment *base);
void Environment_delete (Environment *self);

Expression *Environment_find (Environment *self, Symbol *sym);
Symbol *Environment_reverseFind (Environment *self, Expression *expr);
void Environment_add (Environment *self, Symbol *sym, Expression *expr);
void Environment_set (Environment *self, Symbol *sym, Expression *expr);

#endif /* ENVIRONMENT_H__ */
