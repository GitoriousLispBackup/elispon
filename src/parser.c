#include "utils.h"
#include "expression.h"
#include "pair.h"
#include "symbol.h"
#include "string.h"
#include "number.h"
#include "lexer.h"
#include "parser.h"

struct Parser {
  Lexer *lexer;
  int depth;
};

Parser *
Parser_new (const char *input)
{
  Parser *self = NULL;

  debug_in();

  alloc_one(self);

  self->lexer = Lexer_new(input);
  self->depth = 0;

  debug_out();

  return self;
}

void
Parser_delete (Parser *self)
{
  debug_in();

  Lexer_delete(self->lexer);
  free(self);

  debug_out();
}

/* ----- */

void
Parser_error (Parser *self, char *error)
{
  Utils_fatal("Parser: %s at line %d column %d in %s",
              error, Lexer_line(self->lexer), Lexer_col(self->lexer),
              Lexer_source(self->lexer));
}

Pair *
Parser_parsePair (Parser *self)
{
  Expression *car = NULL, *cdr = NULL;

  if (Lexer_step(self->lexer) == TClosingParen)
    return NULL;

  Lexer_stepback(self->lexer);
  car = Parser_parseExpression(self);

  if (Lexer_step(self->lexer) == TDot) {
    if (Lexer_step(self->lexer) == TClosingParen)
      Parser_error(self, "expected expression");
    else Lexer_stepback(self->lexer);

    cdr = Parser_parseExpression(self);

    if (Lexer_step(self->lexer) != TClosingParen)
      Parser_error(self, "expected closing parenthesis");
    self->depth--;
  }
  else {
    Lexer_stepback(self->lexer);
    cdr = Expression_new(PAIR, Parser_parsePair(self));
  }

  return Pair_new(car, cdr);
}

Expression *
Parser_parseExpression (Parser *self)
{
  Expression *expr = NULL;

  switch (Lexer_step(self->lexer)) {
  case TOpeningParen:
    self->depth++;
    expr = Expression_new(PAIR, Parser_parsePair(self));
    break;
  case TClosingParen:
    self->depth--;
    if (self->depth < 0)
      Parser_error(self, "too many closing parentheses");
    break;
  case TDot:
    Parser_error(self, ". used out of context");
    break;
  case TSymbol:
    expr = Expression_new(SYMBOL, Symbol_new(Lexer_token(self->lexer)));
    break;
  case TString:
    expr = Expression_new(STRING, String_new(Lexer_token(self->lexer)));
    break;
  case TNumber:
    expr = Expression_new(NUMBER, Number_new(Lexer_number(self->lexer)));
    break;
  case TEnd:
    if (self->depth != 0)
      Parser_error(self, "reached EOF before end of expression");
    break;
  default:
    Parser_error(self, "unknown token type");
  }

  return expr;
}
