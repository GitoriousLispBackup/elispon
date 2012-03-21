#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "utils.h"
#include "lexer.h"

#define MAX_BUF_SIZE 2048

struct Lexer {
  FILE *input;
  char *source;
  int line, col;
  int c;
  int size;
  TokenType type;
  char token[MAX_BUF_SIZE];
  double number;
  bool back;
};

Lexer *
Lexer_new (const char *source)
{
  Lexer *self = NULL;

  debug_in();

  alloc_one(self);

  self->input = Utils_openFile(source, "r");
  alloc_(self->source, strlen(source) + 1);
  strcpy(self->source, source);
  self->line = 1;
  self->col = 0;
  self->c = 0;
  self->size = 0;
  self->type = TUnkown;
  memset(self->token, 0, MAX_BUF_SIZE);
  self->number = 0;
  self->back = false;

  debug_out();

  return self;
}

void
Lexer_delete (Lexer *self)
{
  debug_in();

  Utils_closeFile(self->input);
  free(self);

  debug_out();
}

/* ----- */

void
Lexer_error (Lexer *self, char *error)
{
  Utils_fatal("Lexer: %s at line %d column %d in %s",
              error, self->line, self->col, self->source);
}

int
Lexer_getc (Lexer *self)
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
Lexer_ungetc (Lexer *self)
{
  self->col--;
  return ungetc(self->c, self->input);
}

void
Lexer_readToken (Lexer *self)
{
  self->size = 0;

  for (;;) {
    Lexer_getc(self);
    if (self->c == EOF || isspace(self->c) ||
        strchr("()\";", self->c) != NULL) {
      Lexer_ungetc(self);
      self->token[self->size] = '\0';
      return;
    }
    else self->token[self->size++] = self->c;
  }
}

void
Lexer_readString (Lexer *self)
{
  int escaped = 0;

  self->size = 0;
  while (Lexer_getc(self) != EOF) {
    if (escaped) {
      switch (self->c) {
      case '\\':
      case '"':
        escaped = self->c;
        break;
      case '0':
        escaped = '\0';
        break;
      case 'a':
        escaped = '\a';
        break;
      case 'b':
        escaped = '\b';
        break;
      case 't':
        escaped = '\t';
        break;
      case 'n':
        escaped = '\n';
        break;
      case 'v':
        escaped = '\v';
        break;
      case 'f':
        escaped = '\f';
        break;
      case 'r':
        escaped = '\r';
        break;
      }
      self->token[self->size++] = escaped;
      escaped = 0;
    }
    else if (self->c == '\\') escaped = 1;
    else if (self->c == '"') {
      self->token[self->size] = '\0';
      return;
    }
    else {
      if (self->c == '\n') {
        self->line++;
        self->col = 0;
      }
      self->token[self->size++] = self->c;
      if (self->size >= MAX_BUF_SIZE)
        Lexer_error(self, "string or symbol length exceed max buffer size");
    }
  }

  Lexer_error(self, "reached EOF before end of string");
}

/* ----- */

TokenType
Lexer_step (Lexer *self)
{
  if (self->back) {
    self->back = false;
    return self->type;
  }

  self->type = TUnkown;

  while (Lexer_getc(self) != EOF) {
    if (isspace(self->c)) {
      if (self->c == '\n') {
        self->line++;
        self->col = 0;
      }
      continue;
    }

    switch (self->c) {
    case '(':
      self->type = TOpeningParen;
      break;
    case ')':
      self->type = TClosingParen;
      break;
    case '"':
      Lexer_readString(self);
      self->type = TString;
      break;
    default:
      {
        int c = self->c;
        Lexer_ungetc(self);
        Lexer_readToken(self);

        if (self->size == 1 && c == '.')
          self->type = TDot;
        else if (strchr("0123456789+-.", c) != NULL) {
          char *end;

          self->number = strtod(self->token, &end);

          if (self->number == 0 && end == self->token)
            self->type = TSymbol;
          else if (end - self->token == self->size)
            self->type = TNumber;
          else Lexer_error(self, "malformed number");
        }
        else self->type = TSymbol;
      }
    }

    if (self->type != TUnkown)
      return self->type;
  }

  return self->type = TEnd;
}

void
Lexer_stepback (Lexer *self)
{
  self->back = true;
}

TokenType
Lexer_type (Lexer *self)
{
  return self->type;
}

char *
Lexer_token (Lexer *self)
{
  return self->token;
}

double
Lexer_number (Lexer *self)
{
  return self->number;
}

/* ----- */

int
Lexer_line (Lexer *self)
{
  return self->line;
}

int
Lexer_col (Lexer *self)
{
  return self->col;
}

char *
Lexer_source (Lexer *self)
{
  return self->source;
}
