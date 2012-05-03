#ifndef EVAL_H__
#define EVAL_H__

#include "expression.h"
#include "environment.h"
#include "port.h"
#include "symbol-table.h"
#include "parser.h"
#include "printer.h"

typedef struct Eval Eval;

Eval *Eval_new (Port *input, Port *output, Port *errput, SymbolTable *symbols);
void Eval_delete (Eval *self);

Port *Eval_input (Eval *self);
Port *Eval_output (Eval *self);
Port *Eval_errput (Eval *self);
Parser *Eval_parser (Eval *self);
Printer *Eval_printer (Eval *self);

Expression *Eval_eval (Eval *self, Expression *expr, Environment **env);

#endif /* EVAL_H__ */
