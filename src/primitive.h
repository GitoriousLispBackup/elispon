#ifndef PRIMITIVE_H__
#define PRIMITIVE_H__

#include "expression.h"
#include "symbol.h"
#include "environment.h"
#include "eval.h"

typedef struct Primitive Primitive;
typedef Expression *(*PrimitiveProc)(Expression *, Environment **, Eval *);

char *Primitive_name (Primitive *self);
PrimitiveProc Primitive_proc (Primitive *self);

Symbol *Primitive_true ();
Expression *Primitive_initialSymbols ();
Environment *Primitive_initialEnvironment ();

#endif /* PRIMITIVE_H__ */
