#include "utils.h"
#include "character.h"

struct Character {
  char c;
};

Character *
Character_new (char c)
{
  Character *self = NULL;

  alloc_one(self);

  self->c = c;

  return self;
}

void
Character_delete (Character *self)
{
  if (self == NULL) return;
  free_(self);
}

/* ----- */

char
Character_c (Character *self)
{
  return self->c;
}
