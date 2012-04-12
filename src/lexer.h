#ifndef LEXER_H__
#define LEXER_H__

#include "port.h"

typedef struct Lexer Lexer;

typedef enum {
  TOpeningParen, TClosingParen, TDot,
  TSymbol, TCharacter, TString, TNumber,
  TEnd, TUnkown
} TokenType;

Lexer *Lexer_new (Port *input);
void Lexer_delete (Lexer *self);
void Lexer_reset (Lexer *self, Port *input);

TokenType Lexer_type (Lexer *self);
char *Lexer_token (Lexer *self);
double Lexer_number (Lexer *self);
int Lexer_line (Lexer *self);
int Lexer_col (Lexer *self);
Port *Lexer_input (Lexer *self);

TokenType Lexer_step (Lexer *self);
void Lexer_stepback (Lexer *self);

#endif /* LEXER_H__ */
