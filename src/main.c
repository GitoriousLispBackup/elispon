#include <stdio.h>
#include "expression.h"
#include "number.h"
#include "symbol.h"
#include "string.h"
#include "pair.h"

int
main (int argc, char *argv[])
{
  Expression *e = NULL;

  printf("(εlispon) version 0.1\n");

  e = Expression_new(PAIR,
                     Pair_new(Expression_new(SYMBOL, Symbol_new("quote")),
                              Expression_new(NUMBER, Number_new(42))));

  printf("%g\n",
         Number_val(Expression_expr(Pair_snd(Expression_expr(e)))));

  Expression_delete(e);

  return 0;
}
