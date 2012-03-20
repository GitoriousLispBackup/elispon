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

  free_(self);

  debug_out();
}

void
Printer_printPair (Printer *self, Pair *pair)
{
  fprintf(self->output, "(");
  if (pair != NULL) {
    Printer_printExpression(self, Pair_fst(pair));
    fprintf(self->output, " . ");
    Printer_printExpression(self, Pair_snd(pair));
  }
  fprintf(self->output, ")");
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
  void (*printer[4])() = {
    Printer_printPair,
    Printer_printSymbol,
    Printer_printString,
    Printer_printNumber
  };

  printer[Expression_type(expr)](self, Expression_expr(expr));
}
