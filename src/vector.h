#ifndef VECTOR_H__
#define VECTOR_H__

#include "expression.h"

typedef struct Vector Vector;

Vector *Vector_new (int size);
void Vector_delete (Vector *self);

int Vector_size (Vector *self);

void Vector_set (Vector *self, int i, Expression *expr);
Expression *Vector_get (Vector *self, int i);

#endif /* VECTOR_H__ */
