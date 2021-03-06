#include <stdbool.h>
#include "utils.h"
#include "port.h"
#include "expression.h"
#include "pair.h"
#include "symbol.h"
#include "character.h"
#include "string.h"
#include "number.h"
#include "primitive.h"
#include "lexer.h"
#include "symbol-table.h"
#include "parser.h"

struct Parser {
  Lexer *lexer;
  SymbolTable *symbols;
  int depth;
  bool error;
};

Parser *
Parser_new (Port *input, SymbolTable *symbols)
{
  Parser *self = NULL;

  alloc_one(self);

  self->lexer = Lexer_new(input);
  self->symbols = symbols;
  self->depth = 0;
  self->error = false;

  return self;
}

void
Parser_delete (Parser *self)
{
  if (self == NULL) return;
  Lexer_delete(self->lexer);
  free_(self);
}

void
Parser_reset (Parser *self, Port *input)
{
  Lexer_reset(self->lexer, input);
  self->depth = 0;
  self->error = false;
}

/* ----- */

static void *
Parser_error (Parser *self, char *error)
{
  Utils_error("Parser: %s at line %d column %d in %s",
              error, Lexer_line(self->lexer), Lexer_col(self->lexer),
              Port_name(Lexer_input(self->lexer)));
  self->error = true;
  return NULL;
}

static Expression *
Parser_parsePair (Parser *self)
{
  Expression *car = NULL, *cdr = NULL;

  if (Lexer_step(self->lexer) == TClosingParen) {
    self->depth--;
    return Expression_new(NIL, NULL);
  }

  Lexer_stepback(self->lexer);
  car = Parser_parse(self);

  if (Lexer_step(self->lexer) == TDot) {
    if (Lexer_step(self->lexer) == TClosingParen)
      return Parser_error(self, "expected expression");
    else Lexer_stepback(self->lexer);

    cdr = Parser_parse(self);

    if (Lexer_step(self->lexer) != TClosingParen)
      return Parser_error(self, "expected closing parenthesis");
    self->depth--;
  }
  else {
    Lexer_stepback(self->lexer);
    cdr = Parser_parsePair(self);
  }

  return Expression_new(PAIR, Pair_new(car, cdr));
}

Expression *
Parser_parse (Parser *self)
{
  Expression *expr = NULL;

  switch (Lexer_step(self->lexer)) {
  case TOpeningParen:
    self->depth++;
    expr = Parser_parsePair(self);
    break;
  case TClosingParen:
    self->depth--;
    if (self->depth < 0)
      return Parser_error(self, "too many closing parentheses");
    break;
  case TDot:
    return Parser_error(self, ". used out of context");
    break;
  case TSymbol:
    expr = Expression_new(SYMBOL,
                          SymbolTable_find(self->symbols,
                                           Lexer_token(self->lexer)));
    break;
  case TCharacter:
    expr = Expression_new(CHARACTER, Character_new(*Lexer_token(self->lexer)));
    break;
  case TString:
    expr = Expression_new(STRING, String_new(Lexer_token(self->lexer)));
    break;
  case TNumber:
    expr = Expression_new(NUMBER, Number_new(Lexer_number(self->lexer)));
    break;
  case TEnd:
    if (self->depth != 0)
      return Parser_error(self, "reached EOF before end of expression");
    break;
  default:
    return Parser_error(self, "unknown token type");
  }

  if (self->error) {
    Expression_delete(expr);
    return NULL;
  }

  return expr;
}

Expression *
Parser_parseOneExpression (Parser *self)
{
  Expression *expr = Parser_parse(self);

  if (Lexer_step(self->lexer) != TEnd)
    return Parser_error(self, "expected EOF");

  return expr;
}
