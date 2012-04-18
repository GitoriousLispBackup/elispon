#include <stdio.h>
#include "utils.h"
#include "expression.h"
#include "primitive.h"
#include "pair.h"
#include "symbol.h"
#include "character.h"
#include "string.h"
#include "number.h"
#include "fexpr.h"
#include "environment.h"
#include "struct.h"
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
  /* TODO: should print symbol between |s if it contains spaces, quotes… */
}

static void
Printer_printCharacter (Printer *self, Character *c)
{
  switch (Character_c(c)) {
  case '\n':
    Port_printf(self->output, "#\\newline");
    break;
  case '\t':
    Port_printf(self->output, "#\\tab");
    break;
  case ' ':
    Port_printf(self->output, "#\\space");
    break;
  default:
    Port_printf(self->output, "#\\%c", Character_c(c));
  }
}

void
Printer_printExpression (Printer *self, Expression *expr)
{
  switch (Expression_type(expr)) {
  case PRIMITIVE:
    Port_printf(self->output, "#{primitive:%s}",
                Primitive_name(Expression_expr(expr)));
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
  case CHARACTER:
    Printer_printCharacter(self, Expression_expr(expr));
    break;
  case STRING:
    Port_printf(self->output, "\"%s\"", String_buf(Expression_expr(expr)));
    break;
  case NUMBER:
    Port_printf(self->output, "%g", Number_val(Expression_expr(expr)));
    break;
  case FEXPR:
  case ENVIRONMENT:
  case STRUCT:
    Port_printf(self->output, "#{%s}", Expression_typeName(expr));
    break;
  default:
    Utils_error("Printer: unknown expression type");
  }
}
