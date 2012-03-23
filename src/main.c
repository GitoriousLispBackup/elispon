#include <stdio.h>
#include "utils.h"
#include "port.h"
#include "parser.h"
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

  repl = REPL_new(parser, eval, env);
  while (REPL_print(REPL_eval(REPL_read(repl))));
  REPL_delete(repl);

  /*/
  printf("1\n");
  Parser_delete(parser);
  printf("2\n");
  Environment_delete(env);
  printf("3\n");
  Eval_delete(eval);
  /*/

  return 0;
}
