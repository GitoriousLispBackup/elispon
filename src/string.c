#include <string.h>
#include "utils.h"
#include "string.h"

struct String {
  int size;
  char *buf;
};

String *
String_new (const char *str)
{
  String *self = NULL;

  alloc_one(self);

  self->size = strlen(str);
  alloc_(self->buf, self->size + 1);
  strcpy(self->buf, str);

  return self;

}

void
String_delete (String *self)
{
  if (self == NULL) return;
  free_(self->buf);
  free_(self);
}

/* ----- */

char *
String_buf (String *self)
{
  return self->buf;
}

int
String_length (String *self)
{
  return self->size;
}

/* ----- */

char
String_getChar (String *self, int i)
{
  return self->buf[i];
}
