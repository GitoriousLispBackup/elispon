#include "utils.h"
#include "symbol.h"
#include "struct.h"

struct Struct {
  int size;
  Symbol **members;
};

Struct *
Struct_new (int size)
{
  Struct *self = NULL;

  alloc_one(self);

  self->size = size;
  alloc_(self->members, size);

  return self;
}

void
Struct_delete (Struct *self)
{
  if (self == NULL) return;
  free_(self);
}

/* ----- */

int
Struct_size (Struct *self)
{
  return self->size;
}

Symbol **
Struct_members (Struct *self)
{
  return self->members;
}

/* ----- */

void
Struct_declareMember (Struct *self, int position, Symbol *member)
{
  if (position >= self->size)
    Utils_fatal("Struct_declareMember: member position outside struct.");

  self->members[position] = member;
}

int
Struct_memberPosition (Struct *self, Symbol *member)
{
  int position;

  for (position = 0; position < self->size; position++)
    if (member == self->members[position])
      return position;

  return -1;
}
