#include <stdio.h>
#include <ctype.h>
#include "utils.h"
#include "expression.h"
#include "pair.h"
#include "symbol.h"
#include "string.h"
#include "number.h"
#include "printer.h"

struct Printer {
  FILE *output;
  int indent;
};

Printer *
Printer_new (const char *output)
{
  Printer *self = NULL;

  debug_in();

  alloc_one(self);

  self->output = Utils_openFile(output, "a");
  self->indent = 0;

  debug_out();

  return self;
}

void
Printer_delete (Printer *self)
{
  debug_in();

  Utils_closeFile(self->output);
  free_(self);

  debug_out();
}

/* ----- */

void
Printer_printPair (Printer *self, Pair *pair)
{
  Printer_printExpression(self, Pair_fst(pair));
  if (Expression_type(Pair_snd(pair)) == PAIR) {
    if (!Expression_isNil(Pair_snd(pair))) {
      fprintf(self->output, " ");
      Printer_printPair(self, Expression_expr(Pair_snd(pair)));
    }
  }
  else {
    fprintf(self->output, " . ");
    Printer_printExpression(self, Pair_snd(pair));
  }
}

void
Printer_printSymbol (Printer *self, Symbol *sym)
{
  fprintf(self->output, Symbol_name(sym));
}

void
Printer_printString (Printer *self, String *str)
{
  fprintf(self->output, "\"");
  fprintf(self->output, String_buf(str));
  fprintf(self->output, "\"");
}

void
Printer_printNumber (Printer *self, Number *num)
{
  fprintf(self->output, "%g", Number_val(num));
}

void
Printer_printExpression (Printer *self, Expression *expr)
{
  switch (Expression_type(expr)) {
  case PAIR:
    fprintf(self->output, "(");
    if (!Expression_isNil(expr))
      Printer_printPair(self, Expression_expr(expr));
    fprintf(self->output, ")");
    break;
  case SYMBOL:
    Printer_printSymbol(self, Expression_expr(expr));
    break;
  case STRING:
    Printer_printString(self, Expression_expr(expr));
    break;
  case NUMBER:
    Printer_printNumber(self, Expression_expr(expr));
    break;
  default:
    Utils_error("Printer_printExpression: unknown expression type.");
  }
}
