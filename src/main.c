#include <stdio.h>
#include "utils.h"
#include "port.h"
#include "parser.h"
#include "printer.h"
#include "expression.h"
#include "primitive.h"
#include "eval.h"
#include "symbol-table.h"
#include "environment.h"
#include "repl.h"

int
main (int argc, char *argv[])
{
  FILE *stdlib = NULL;
  Port *libin = NULL, *input = NULL, *output = NULL, *errput = NULL;
  Parser *parser = NULL;
  Expression *expr = NULL;
  SymbolTable *symbols = NULL;
  Environment *env = NULL;
  Printer *printer = NULL;
  Eval *eval = NULL;
  REPL *repl = NULL;

  symbols = SymbolTable_new(Primitive_initialSymbols());
  parser = Parser_new(NULL, symbols);
  env = Environment_new(Primitive_initialEnvironment(symbols));
  input = Port_newFile(stdin, "stdin");
  output = Port_newFile(stdout, "stdout");
  printer = Printer_new(output);
  errput = Port_newFile(stderr, "stderr");
  eval = Eval_new(input, output, errput, symbols);

  if (argc == 2) {
    stdlib = Utils_openFile(argv[1], "r");
    libin = Port_newFile(stdlib, argv[1]);
    Parser_reset(parser, libin);
    while ((expr = Parser_parse(parser)) != NULL)
      Eval_eval(eval, expr, &env);
    Port_delete(libin);
    Utils_closeFile(stdlib);
  }


  repl = REPL_new(parser, eval, env, printer);
  while (REPL_print(REPL_eval(REPL_read(repl))));
  REPL_delete(repl);

  Printer_delete(printer);
  Port_delete(output);

  return 0;
}
