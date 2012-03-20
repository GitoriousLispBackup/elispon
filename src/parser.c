#include <stdio.h>
#include <ctype.h>
#include "utils.h"
#include "expression.h"
#include "pair.h"
#include "symbol.h"
#include "string.h"
#include "number.h"
#include "parser.h"

#define MAX_BUF_SIZE 2048

struct Parser {
  FILE *input;
  char *input_path;
  int line, col, depth;
  int c, i;
  char buf[MAX_BUF_SIZE];
};

Parser *
Parser_new (const char *input)
{
  Parser *self = NULL;

  debug_in();

  alloc_one(self);

  self->input = Utils_openFile(input, "r");
  alloc_(self->input_path, strlen(input) + 1);
  strcpy(self->input_path, input);
  self->line = 1;
  self->col = 0;
  self->depth = 0;
  self->c = EOF;
  memset(self->buf, 0, MAX_BUF_SIZE);

  debug_out();

  return self;
}

void
Parser_delete (Parser *self)
{
  debug_in();

  Utils_closeFile(self->input);
  free(self);

  debug_out();
}

void
Parser_error (Parser *self, char *error)
{
  Utils_fatal("Parser: %s at line %d column %d in %s",
              error, self->line, self->col, self->input_path);
}

int
Parser_getc (Parser *self)
{
  self->col++;
  self->c = getc(self->input);

  while (self->c == ';') {
    while (getc(self->input) != '\n');
    self->line++;
    self->col = 1;
    self->c = getc(self->input);
  }

  return self->c;
}

int
Parser_ungetc (Parser *self)
{
  self->col--;
  return ungetc(self->c, self->input);
}

void
Parser_readUntil (Parser *self, char terminator)
{
  int escaped = 0;

  self->i = 0;
  while (Parser_getc(self) != EOF) {
    if (escaped) {
      switch (self->c) {
      case '\\':
      case '"':
      case '|':
        escaped = self->c;
        break;
      case '0':
        escaped = '\0';
        break;
      case 'f':
        escaped = '\f';
        break;
      case 'n':
        escaped = '\n';
        break;
      case 'r':
        escaped = '\r';
        break;
      case 't':
        escaped = '\t';
        break;
      }
      self->buf[self->i++] = escaped;
      escaped = 0;
    }
    else if (self->c == '\\') escaped = 1;
    else if (self->c == terminator) {
      self->buf[self->i] = '\0';
      return;
    }
    else {
      if (self->c == '\n') {
        self->line++;
        self->col = 0;
      }
      self->buf[self->i++] = self->c;
      if (self->i >= MAX_BUF_SIZE)
        Parser_error(self, "self->igth of string or symbol exceed buffer size");
    }
  }

  Parser_error(self, "reached EOF before end of string or symbol");
}

#define isseparator(c) \
  (isspace(c) || c == '(' || c == ')' || c == '"' || c == ';' || c == EOF)

void
Parser_readToken (Parser *self)
{
  self->i = 0;
  for (;;) {
    Parser_getc(self);
    if (isseparator(self->c)) {
      Parser_ungetc(self);
      self->buf[self->i] = '\0';
      return;
    }
    else self->buf[self->i++] = self->c;
  }
}

#undef isseparator

Pair *
Parser_parsePair (Parser *self)
{
  if (Parser_getc(self) == ')') return NULL;
  Parser_ungetc(self);

  Expression *car = NULL, *cdr = NULL;

  car = Parser_parseExpression(self);
  cdr = Expression_new(PAIR, Parser_parsePair(self));

  return Pair_new(car, cdr);
}

Symbol *
Parser_parseSymbol (Parser *self)
{
  if (Parser_getc(self) == '|')
    Parser_readUntil(self, '|');
  else {
    Parser_ungetc(self);
    Parser_readToken(self);
  }
  return Symbol_new(self->buf);
}

String *
Parser_parseString (Parser *self)
{
  Parser_readUntil(self, '"');
  return String_new(self->buf);
}

Expression *
Parser_parseExpression (Parser *self)
{
  while (Parser_getc(self) != EOF) {
    if (isspace(self->c)) {
      if (self->c == '\n') {
        self->line++;
        self->col = 0;
      }
      continue;
    }

    if (self->c == '(') {
      self->depth++;
      return Expression_new(PAIR, Parser_parsePair(self));
    }

    if (self->c == ')') {
      self->depth--;
      if (self->depth < 0)
        Parser_error(self, "no match for closing parenthesis");
      return NULL;
    }

    if (self->c == '"') {
      return Expression_new(STRING, Parser_parseString(self));
    }

    {
      double num;
      char *end;
      
      Parser_ungetc(self);
      Parser_readToken(self);
      
      num = strtod(self->buf, &end);
      if (num == 0 && end == self->buf)
        return Expression_new(SYMBOL, Symbol_new(self->buf));
      else {
        if (end - self->buf != self->i)
          Parser_error(self, "Malformed number");
        else
          return Expression_new(NUMBER, Number_new(num));
      }
    }

    return Expression_new(SYMBOL, Parser_parseSymbol(self));
  }

  if (self->depth > 0)
    Parser_error(self, "EOF reached while reading expression");

  return NULL;
}
