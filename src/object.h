#ifndef OBJECT_H__
#define OBJECT_H__

#include <stdbool.h>
#include "struct.h"
#include "symbol.h"
#include "expression.h"

typedef struct Object Object;

Object *Object_new (Struct *type, const char name[SYMBOL_NAME_MAX_SIZE]);
void Object_delete (Object *self);

char *Object_name (Object *self);
Struct *Object_type (Object *self);
Expression **Object_fields (Object *self);

bool Object_setField (Object *self, Symbol *field, Expression *expr);
void Object_setFieldByPosition (Object *self, int position, Expression *expr);
Expression *Object_field (Object *self, Symbol *field);

#endif /* OBJECT_H__ */
