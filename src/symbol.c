#include <string.h>
#include "utils.h"
#include "symbol.h"

struct Symbol {
  char name[SYMBOL_NAME_MAX_SIZE];
};

Symbol *
Symbol_new (const char name[SYMBOL_NAME_MAX_SIZE])
{
  Symbol *self = NULL;

  alloc_one(self);

  strncpy(self->name, name, SYMBOL_NAME_MAX_SIZE - 1);
  self->name[SYMBOL_NAME_MAX_SIZE - 1] = '\0';

  return self;
}

void
Symbol_delete (Symbol *self)
{
  if (self == NULL) return;
  free_(self);
}

/* ----- */

const char *
Symbol_name (Symbol *self)
{
  return self->name;
}
