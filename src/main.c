#include <stdio.h>
#include "utils.h"
#include "port.h"
#include "parser.h"
#include "printer.h"
#include "expression.h"
#include "primitive.h"
#include "eval.h"
#include "environment.h"
#include "repl.h"

int
main (int argc, char *argv[])
{
  FILE *stdlib = NULL;
  Port *port = NULL;
  Parser *parser = Parser_new(NULL, Primitive_initialSymbols());
  Expression *expr = NULL;
  Environment *env = Environment_new(Primitive_initialEnvironment());
  Eval *eval = Eval_new(env);
  Printer *printer = NULL;
  REPL *repl = NULL;

  if (argc == 2) {
    stdlib = Utils_openFile(argv[1], "r");
    port = Port_newFile(stdlib, "stdlib");
    Parser_reset(parser, port);
    while ((expr = Parser_parseExpression(parser)) != NULL)
      Eval_eval(eval, expr, &env);
    Port_delete(port);
    Utils_closeFile(stdlib);
  }

  port = Port_newFile(stdout, "stdout");
  printer = Printer_new(port);
  repl = REPL_new(parser, eval, env, printer);
  while (REPL_print(REPL_eval(REPL_read(repl))));
  REPL_delete(repl);
  Printer_delete(printer);
  Port_delete(port);

  return 0;
}
