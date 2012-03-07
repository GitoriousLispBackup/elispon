#ifndef EXPRESSION_H__
#define EXPRESSION_H__

#include <stdio.h>

typedef struct Expression Expression;

typedef enum { NUMBER, SYMBOL, STRING, PAIR } ExprType;

Expression *Expression_new (ExprType type, void *expr);
void Expression_delete (Expression *self);

ExprType Expression_type (Expression *self);
void *Expression_expr (Expression *self);

#endif /* EXPRESSION_H__ */
