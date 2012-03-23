#ifndef ENVIRONMENT_H__
#define ENVIRONMENT_H__

#include "expression.h"
#include "symbol.h"

typedef struct Expression Environment;

Environment *Environment_new (Environment *base);
void Environment_delete (Environment *self);

Expression *Environment_find (Environment *self, Symbol *sym);
Symbol *Environment_revFind (Environment *self, Expression *value);
Environment *Environment_add (Environment *self, Symbol *sym, Expression *expr);

#endif /* ENVIRONMENT_H__ */
