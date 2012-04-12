#include <stdio.h>
#include "utils.h"
#include "expression.h"
#include "primitive.h"
#include "pair.h"
#include "symbol.h"
#include "string.h"
#include "number.h"
#include "fexpr.h"
#include "port.h"
#include "printer.h"

struct Printer {
  Port *output;
  int indent;
};

Printer *
Printer_new (Port *output)
{
  Printer *self = NULL;

  alloc_one(self);

  self->output = output;
  self->indent = 0;

  return self;
}

void
Printer_delete (Printer *self)
{
  if (self == NULL) return;
  free_(self);
}

/* ----- */

static void
Printer_printPrimitive (Printer *self, Primitive *prim)
{
  Port_printf(self->output, "#{primitive:%s}", Primitive_name(prim));
}

static void
Printer_printInfiniteList (Printer *self, Pair *pair, Pair *start)
{
  if (pair == start) {
    if (Pair_flag(pair)) {
      Pair_setFlag(pair, false);
      Port_printf(self->output, "…]");
      return;
    }
    else Port_printf(self->output, "[");
  }

  Pair_setFlag(pair, true);
  if (Expression_type(Pair_fst(pair)) == PAIR &&
      Expression_expr(Pair_fst(pair)) == start) {
    Pair_setFlag(pair, false);
    Port_printf(self->output, "…]");
    return;
  }
  else Printer_printExpression(self, Pair_fst(pair));
  Port_printf(self->output, " ");
  Printer_printInfiniteList(self, Expression_expr(Pair_snd(pair)), start);
  Pair_setFlag(pair, false);
}

static void
Printer_printPair (Printer *self, Pair *pair)
{
  if (Pair_flag(pair)) {
    Pair_setFlag(pair, false);
    Printer_printInfiniteList(self, pair, pair);
    return;
  }

  Pair_setFlag(pair, true);
  Printer_printExpression(self, Pair_fst(pair));

  if (Expression_type(Pair_snd(pair)) == PAIR) {
    Port_printf(self->output, " ");
    Printer_printPair(self, Expression_expr(Pair_snd(pair)));
  }
  else if (Expression_type(Pair_snd(pair)) != NIL) {
    Port_printf(self->output, " . ");
    Printer_printExpression(self, Pair_snd(pair));
  }
  Pair_setFlag(pair, false);
}

static void
Printer_printSymbol (Printer *self, Symbol *sym)
{
  Port_printf(self->output, "%s", Symbol_name(sym));
}

static void
Printer_printString (Printer *self, String *str)
{
  Port_printf(self->output, "\"%s\"", String_buf(str));
}

static void
Printer_printNumber (Printer *self, Number *num)
{
  Port_printf(self->output, "%g", Number_val(num));
}

static void
Printer_printFexpr (Printer *self, Fexpr *fexpr)
{
  Port_printf(self->output, "#{fexpr}");
}

static void
Printer_printEnvironment (Printer *self, Environment *env)
{
  Port_printf(self->output, "#{environment}");
}

void
Printer_printExpression (Printer *self, Expression *expr)
{
  switch (Expression_type(expr)) {
  case PRIMITIVE:
    Printer_printPrimitive(self, Expression_expr(expr));
    break;
  case NIL:
    Port_printf(self->output, "()");
    break;
  case PAIR:
    Port_printf(self->output, "(");
    Printer_printPair(self, Expression_expr(expr));
    Port_printf(self->output, ")");
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
  case ENVIRONMENT:
    Printer_printEnvironment(self, Expression_expr(expr));
    break;
  default:
    Utils_error("Printer: unknown expression type.");
  }
}
