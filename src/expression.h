#ifndef EXPRESSION_H__
#define EXPRESSION_H__

#include <stdbool.h>

typedef struct Expression Expression;

typedef enum {
  PRIMITIVE, NIL, PAIR, SYMBOL, CHARACTER, STRING, NUMBER,
  FEXPR, ENVIRONMENT, STRUCT
} ExprType;

Expression *Expression_new (ExprType type, void *expr);
void Expression_delete (Expression *self);

ExprType Expression_type (Expression *self);
void Expression_setType (Expression *self, ExprType type);
void *Expression_expr (Expression *self);
void Expression_setExpr (Expression *self, void *expr);

char *Expression_typeName (Expression *self);
bool Expression_isNil (Expression *self);
bool Expression_isCallable (Expression *self);
Expression *Expression_cons (Expression *car, Expression *cdr);
Expression *Expression_car (Expression *self);
Expression *Expression_cdr (Expression *self);
void Expression_setCar (Expression *self, Expression *expr);
void Expression_setCdr (Expression *self, Expression *expr);
int Expression_length (Expression *self);

#endif /* EXPRESSION_H__ */
