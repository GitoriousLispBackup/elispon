#include <string.h>
#include "utils.h"
#include "string.h"

struct String {
  int size;
  char *buf;
};

String *
String_new (char *str)
{
  String *self = NULL;

  debug_in();

  alloc_one(self);

  self->size = strlen(str) + 1;
  alloc_(self->buf, self->size);
  strcpy(self->buf, str);

  debug_out();

  return self;

}

void
String_delete (String *self)
{
  debug_in();
  
  free_(self->buf);
  free_(self);

  debug_out();
}

/* ----- */

char *
String_buf (String *self)
{
  return self->buf;
}
