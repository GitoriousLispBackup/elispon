#ifndef PARSER_H__
#define PARSER_H__

#include "expression.h"

typedef struct Parser Parser;

Parser *Parser_new (const char *input);
void Parser_delete (Parser *self);

Expression *Parser_parseExpression (Parser *self);

#endif /* PARSER_H__ */
