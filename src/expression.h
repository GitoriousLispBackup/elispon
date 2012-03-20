#ifndef EXPRESSION_H__
#define EXPRESSION_H__

typedef struct Expression Expression;

typedef enum { PAIR, SYMBOL, STRING, NUMBER } ExprType;

Expression *Expression_new (ExprType type, void *expr);
void Expression_delete (Expression *self);

ExprType Expression_type (Expression *self);
void *Expression_expr (Expression *self);

#endif /* EXPRESSION_H__ */
