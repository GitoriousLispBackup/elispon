#ifndef PARSER_H__
#define PARSER_H__

#include "port.h"
#include "expression.h"
#include "symbol-table.h"

typedef struct Parser Parser;

Parser *Parser_new (Port *input, SymbolTable *symbols);
void Parser_delete (Parser *self);
void Parser_reset (Parser *self, Port *input);

Expression *Parser_parse (Parser *self);
Expression *Parser_parseOneExpression (Parser *self);

#endif /* PARSER_H__ */
