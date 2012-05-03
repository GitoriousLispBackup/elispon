#ifndef EVAL_H__
#define EVAL_H__

#include "expression.h"
#include "environment.h"
#include "port.h"
#include "printer.h"

typedef struct Eval Eval;

Eval *Eval_new (Port *input, Port *output, Port *errput);
void Eval_delete (Eval *self);

Port *Eval_input (Eval *self);
Port *Eval_output (Eval *self);
Port *Eval_errput (Eval *self);
Printer *Eval_printer (Eval *self);

Expression *Eval_eval (Eval *self, Expression *expr, Environment **env);

#endif /* EVAL_H__ */
