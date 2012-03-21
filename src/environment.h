#ifndef ENVIRONMENT_H__
#define ENVIRONMENT_H__

#include "expression.h"
#include "symbol.h"

typedef struct Expression Environment;

Environment *Environment_new ();
void Environment_delete (Environment *self);

Expression *Environment_find (Environment *self, Symbol *sym);
Environment *Environment_set (Environment *self, Symbol *sym, Expression *expr);

#endif /* ENVIRONMENT_H__ */
