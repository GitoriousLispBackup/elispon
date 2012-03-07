#include <string.h>
#include "utils.h"
#include "symbol.h"

struct Symbol {
  char name[MAX_SYMBOL_LENGTH];
};

Symbol *
Symbol_new (char name[MAX_SYMBOL_LENGTH])
{
  Symbol *self = NULL;

  debug_in();

  alloc_one(self);

  strncpy(self->name, name, MAX_SYMBOL_LENGTH);

  debug_out();

  return self;
}

void
Symbol_delete (Symbol *self)
{
  debug_in();

  free_(self);

  debug_out();
}

/* ----- */

char *
Symbol_name (Symbol *self)
{
  return self->name;
}
