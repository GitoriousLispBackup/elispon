#include <stdio.h>
#include "utils.h"
#include "expression.h"
#include "primitive.h"
#include "pair.h"
#include "symbol.h"
#include "string.h"
#include "number.h"
#include "fexpr.h"
#include "printer.h"

struct Printer {
  FILE *output;
  int indent;
};

Printer *
Printer_new (const char *output)
{
  Printer *self = NULL;

  alloc_one(self);

  self->output = Utils_openFile(output, "a");
  self->indent = 0;

  return self;
}

void
Printer_delete (Printer *self)
{
  if (self == NULL) return;
  Utils_closeFile(self->output);
  free_(self);
}

/* ----- */

static void
Printer_printPrimitive (Printer *self, Primitive *prim)
{
  fprintf(self->output, "#<primitive:%s>", Primitive_name(prim));
}

static void
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

static void
Printer_printSymbol (Printer *self, Symbol *sym)
{
  fprintf(self->output, Symbol_name(sym));
}

static void
Printer_printString (Printer *self, String *str)
{
  fprintf(self->output, "\"");
  fprintf(self->output, String_buf(str));
  fprintf(self->output, "\"");
}

static void
Printer_printNumber (Printer *self, Number *num)
{
  fprintf(self->output, "%g", Number_val(num));
}

static void
Printer_printFexpr (Printer *self, Fexpr *fexpr)
{
  fprintf(self->output, "#<fexpr>");
}

void
Printer_printExpression (Printer *self, Expression *expr)
{
  switch (Expression_type(expr)) {
  case PRIMITIVE:
    Printer_printPrimitive(self, Expression_expr(expr));
    break;
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
  case FEXPR:
    Printer_printFexpr(self, Expression_expr(expr));
    break;
  default:
    Utils_error("Printer: unknown expression type.");
  }

  fflush(self->output);
}
