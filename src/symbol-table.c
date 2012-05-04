#include "utils.h"
#include "symbol.h"
#include "primitive.h"
#include "symbol-table.h"

struct SymbolTable {
  int size, count;
  Symbol **table;
};

SymbolTable *
SymbolTable_new (char **primitives)
{
  int i;
  SymbolTable *self = NULL;

  alloc_one(self);

  self->size = 1024;

  alloc_(self->table, self->size);
  for (i = 0; i < PRIMITIVE_COUNT; i++)
    self->table[i] = Symbol_new(primitives[i]);
  self->table[i] = Primitive_true();

  self->count = i + 1;

  return self;
}

void
SymbolTable_delete (SymbolTable *self)
{
  if (self == NULL) return;
  free_(self->table);
  free_(self);
}

/* ----- */

Symbol *
SymbolTable_find (SymbolTable *self, const char *name)
{
  int i;

  for (i = self->count - 1; i >= 0; i--)
    if (strcmp(Symbol_name(self->table[i]), name) == 0)
      return self->table[i];

  if (self->count >= self->size - 1) {
    self->size *= 2;
    self->table = realloc_(self->table, self->size);
  }

  self->table[self->count] = Symbol_new(name);

  return self->table[self->count++];
}
