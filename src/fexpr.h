#ifndef FEXPR_H__
#define FEXPR_H__

#include "symbol.h"
#include "expression.h"
#include "environment.h"

typedef struct Fexpr Fexpr;

Fexpr *Fexpr_new (Symbol *arg, Symbol *dynenv,
                  Expression *body, Environment *lexenv);
void Fexpr_delete (Fexpr *self);

Symbol *Fexpr_arg (Fexpr *self);
Symbol *Fexpr_dynenv (Fexpr *self);
Expression *Fexpr_body (Fexpr *self);
Expression *Fexpr_lexenv (Fexpr *self);

#endif /* FEXPR_H__ */
