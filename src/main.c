#include <stdio.h>
#include "expression.h"
#include "number.h"
#include "symbol.h"
#include "string.h"
#include "pair.h"
#include "parser.h"
#include "printer.h"

int
main (int argc, char *argv[])
{
  Expression *e = NULL;
  Parser *p = NULL;
  Printer *pr = NULL;

  printf("(εlispon) version 0\n");

  p = Parser_new("/dev/stdin");

  e = Parser_parseExpression(p);

  Parser_delete(p);

  pr = Printer_new("/dev/stdout");

  Printer_printExpression(pr, e);

  Printer_delete(pr);
  printf("\n");

  Expression_delete(e);

  return 0;
}
