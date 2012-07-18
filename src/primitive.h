#ifndef PRIMITIVE_H__
#define PRIMITIVE_H__

#include "expression.h"
#include "symbol.h"
#include "symbol-table.h"
#include "environment.h"
#include "eval.h"

#define PRIMITIVE_COUNT 51

typedef struct Primitive Primitive;
typedef Expression *(*PrimitiveProc)(Expression *, Environment **, Eval *);

char *Primitive_name (Primitive *self);
PrimitiveProc Primitive_proc (Primitive *self);

Symbol *Primitive_true ();
char **Primitive_initialSymbols ();
Environment *Primitive_initialEnvironment (SymbolTable *symbols);

#endif /* PRIMITIVE_H__ */
