#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "utils.h"
#include "port.h"
#include "lexer.h"

#define BUFFER_MAX_SIZE 2048

struct Lexer {
  Port *input;
  int line, col;
  int c;
  int size;
  TokenType type;
  char token[BUFFER_MAX_SIZE];
  double number;
  bool back;
};

Lexer *
Lexer_new (Port *input)
{
  Lexer *self = NULL;

  alloc_one(self);

  self->input = input;
  self->line = 1;
  self->col = 0;
  self->c = 0;
  self->size = 0;
  self->type = TUnkown;
  memset(self->token, 0, BUFFER_MAX_SIZE);
  self->number = 0;
  self->back = false;

  return self;
}

void
Lexer_delete (Lexer *self)
{
  if (self == NULL) return;
  free_(self);
}

void
Lexer_reset (Lexer *self, Port *input)
{
  self->input = input;
  self->line = 1;
  self->col = 0;
  self->c = 0;
  self->size = 0;
  self->type = TUnkown;
  memset(self->token, 0, BUFFER_MAX_SIZE);
  self->number = 0;
  self->back = false;
}

/* ----- */

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

Port *
Lexer_input (Lexer *self)
{
  return self->input;
}

/* ----- */

static void
Lexer_error (Lexer *self, char *error)
{
  Utils_error("Lexer: %s at line %d column %d in %s",
              error, self->line, self->col, Port_name(self->input));
}

static int
Lexer_getc (Lexer *self)
{
  self->col++;
  self->c = Port_getc(self->input);

  while (self->c == ';') {
    while (Port_getc(self->input) != '\n');
    self->line++;
    self->col = 1;
    self->c = Port_getc(self->input);
  }

  return self->c;
}

static int
Lexer_ungetc (Lexer *self)
{
  self->col--;
  return Port_ungetc(self->input, self->c);
}

static void
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

static bool
Lexer_readCharacter (Lexer *self)
{
  if (Lexer_getc(self) != '\\') {
    Lexer_error(self, "malformed character");
    return false;
  }

  Lexer_readToken(self);

  if (self->size != 1) {
    if (strcmp(self->token, "newline") == 0)
      self->token[0] = '\n';
    else if (strcmp(self->token, "tab") == 0)
      self->token[0] = '\t';
    else if (strcmp(self->token, "space") == 0)
      self->token[0] = ' ';
    else {
      Lexer_error(self, "unkown character name");
      return false;
    }
    self->size = 1;
    self->token[1] = '\0';
  }

  return true;
}

static bool
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
      return true;
    }
    else {
      if (self->c == '\n') {
        self->line++;
        self->col = 0;
      }
      self->token[self->size++] = self->c;
      if (self->size >= BUFFER_MAX_SIZE) {
        Lexer_error(self, "string or symbol length exceed max buffer size");
        return false;
      }
    }
  }

  Lexer_error(self, "reached EOF before end of string");
  return false;
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
    case '#':
      if (Lexer_readCharacter(self))
        self->type = TCharacter;
      else return TUnkown;
      break;
    case '"':
      if (Lexer_readString(self))
        self->type = TString;
      else return TUnkown;
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
          else {
            Lexer_error(self, "malformed number");
            return TUnkown;
          }
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
