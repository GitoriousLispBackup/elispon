#ifndef LEXER_H__
#define LEXER_H__

typedef struct Lexer Lexer;

typedef enum {
  TOpeningParen, TClosingParen, TDot,
  TSymbol, TString, TNumber,
  TEnd, TUnkown
} TokenType;

Lexer *Lexer_new (const char *input);
void Lexer_delete (Lexer *self);

TokenType Lexer_step (Lexer *self);
void Lexer_stepback (Lexer *self);
TokenType Lexer_type (Lexer *self);
char *Lexer_token (Lexer *self);
double Lexer_number (Lexer *self);

int Lexer_line (Lexer *self);
int Lexer_col (Lexer *self);
char *Lexer_source (Lexer *self);

#endif /* LEXER_H__ */
