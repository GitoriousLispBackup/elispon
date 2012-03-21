#include <stdio.h>
#include <stdbool.h>
#include "utils.h"
#include "port.h"
#include "expression.h"
#include "parser.h"
#include "printer.h"
#include "repl.h"

#define BUFFER_MAX_SIZE 8192

struct REPL {
  int count;
  int pos;
  char buf[BUFFER_MAX_SIZE];
  Parser *parser;
  Printer *printer;
  Expression *expr;
};

REPL *
REPL_new ()
{
  REPL *self = NULL;

  alloc_one(self);

  self->count = 0;
  memset(self->buf, 0, BUFFER_MAX_SIZE);
  self->parser = Parser_new(NULL);
  self->printer = Printer_new("/dev/stdout");
  self->expr = NULL;

  printf("Welcome to (εlispon) v0.\n");

  return self;
}

void
REPL_delete (REPL *self)
{
  if (self == NULL) return;
  free_(self);

  printf("\n\nBye.\n");
}

REPL *
REPL_read (REPL *self)
{
  int c, depth = 0, line = 1;
  bool str = false, escaped = false;
  char name[64];
  Port *in = NULL;

  self->count++;
  self->pos = 0;

  printf("\nεlispon:%d> ", self->count);
  for (;;) {
    c = getc(stdin);

    if (c == EOF) return NULL;

    if (!str && !escaped && c == '"') str = true;
    else if (str && !escaped && c == '"') str = false;
    else if (!str && !escaped && c == '(') depth++;
    else if (!str && !escaped && c == ')') depth--;

    if (!str && !escaped && c == ';')
      while ((c = getc(stdin)) != '\n');

    if (!escaped && c == '\n') {
      if (depth <= 0) break;
      else {
        line++;
        printf("(%d):%d> ", depth, line);
      }
    }

    if (c == '\\') escaped = true;
    else if (escaped) escaped = false;

    self->buf[self->pos++] = c;
  }
  self->buf[self->pos] = '\0';

  sprintf(name, "repl:%d", self->count);
  in = Port_newBuffer(self->buf, name);
  Parser_reset(self->parser, in);
  self->expr = Parser_parseOneExpression(self->parser);
  Port_delete(in);

  return self;
}

REPL *
REPL_eval (REPL *self)
{
  if (self == NULL) return NULL;
  return self;
}

REPL *
REPL_print (REPL *self)
{
  if (self == NULL) return NULL;

  if (self->expr != NULL)
    Printer_printExpression(self->printer, self->expr);

  return self;
}
