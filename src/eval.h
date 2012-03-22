#ifndef EVAL_H__
#define EVAL_H__

#include "expression.h"
#include "environment.h"

typedef struct Eval Eval;

Eval *Eval_new ();
void Eval_delete (Eval *self);

Expression *Eval_eval (Eval *self, Expression *expr, Environment **env);
Expression *Eval_mapEval (Eval *self, Expression *expr, Environment **env);

#endif /* EVAL_H__ */
