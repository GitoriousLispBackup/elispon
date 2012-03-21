#include <string.h>
#include "utils.h"
#include "symbol.h"

struct Symbol {
  char name[SYMBOL_NAME_MAX_SIZE];
};

Symbol *
Symbol_new (char name[SYMBOL_NAME_MAX_SIZE])
{
  Symbol *self = NULL;

  alloc_one(self);

  strncpy(self->name, name, SYMBOL_NAME_MAX_SIZE);

  return self;
}

void
Symbol_delete (Symbol *self)
{
  if (self == NULL) return;
  free_(self);
}

/* ----- */

char *
Symbol_name (Symbol *self)
{
  return self->name;
}
