#include <stdbool.h>
#include "utils.h"
#include "struct.h"
#include "symbol.h"
#include "expression.h"
#include "object.h"

struct Object {
  char name[SYMBOL_NAME_MAX_SIZE];
  Struct *type;
  Expression **fields;
};

Object *
Object_new (Struct *type, const char name[SYMBOL_NAME_MAX_SIZE])
{
  Object *self = NULL;

  alloc_one(self);

  strncpy(self->name, name, SYMBOL_NAME_MAX_SIZE);
  self->type = type;

  alloc_(self->fields, Struct_size(type));

  return self;
}

void
Object_delete (Object *self)
{
  if (self == NULL) return;
  free_(self->fields);
  free_(self);
}

/* ----- */

char *
Object_name (Object *self)
{
  return self->name;
}

Struct *
Object_type (Object *self)
{
  return self->type;
}

Expression **
Object_fields (Object *self)
{
  return self->fields;
}

/* ----- */

bool
Object_setField (Object *self, Symbol *field, Expression *expr)
{
  int position;

  if ((position = Struct_memberPosition(self->type, field)) < 0)
    return false;

  self->fields[position] = expr;

  return true;
}

void
Object_setFieldByPosition (Object *self, int position, Expression *expr)
{
  self->fields[position] = expr;
}

Expression *
Object_field (Object *self, Symbol *field)
{
  int position;

  if ((position = Struct_memberPosition(self->type, field)) < 0)
    return NULL;

  return self->fields[position];
}
