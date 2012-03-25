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
  Port *input = NULL, *output = NULL, *errput = NULL;
  Parser *parser = NULL;
  Expression *expr = NULL;
  Environment *env = NULL;
  Printer *printer = NULL;
  Eval *eval = NULL;
  REPL *repl = NULL;

  parser = Parser_new(NULL, Primitive_initialSymbols());
  env = Environment_new(Primitive_initialEnvironment());
  output = Port_newFile(stdout, "stdout");
  printer = Printer_new(output);
  errput = Port_newFile(stderr, "stderr");
  eval = Eval_new(errput);

  if (argc == 2) {
    stdlib = Utils_openFile(argv[1], "r");
    input = Port_newFile(stdlib, "stdlib");
    Parser_reset(parser, input);
    while ((expr = Parser_parseExpression(parser)) != NULL)
      Eval_eval(eval, expr, &env);
    Port_delete(input);
    Utils_closeFile(stdlib);
  }


  repl = REPL_new(parser, eval, env, printer);
  while (REPL_print(REPL_eval(REPL_read(repl))));
  REPL_delete(repl);

  Printer_delete(printer);
  Port_delete(output);

  return 0;
}
