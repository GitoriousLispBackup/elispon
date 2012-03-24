#ifndef REPL_H__
#define REPL_H__

#include "parser.h"
#include "eval.h"
#include "environment.h"
#include "printer.h"

typedef struct REPL REPL;

REPL *REPL_new (Parser *parser, Eval *eval, Environment *env, Printer *printer);
void REPL_delete (REPL *self);

REPL *REPL_read (REPL *self);
REPL *REPL_eval (REPL *self);
REPL *REPL_print (REPL *self);

#endif /* REPL_H__ */
