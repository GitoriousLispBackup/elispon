#include <string.h>
#include "utils.h"
#include "string.h"

struct String {
  int length;
  char *buf;
};

String *
String_new (const char *str)
{
  String *self = NULL;

  alloc_one(self);

  self->length = strlen(str);
  alloc_(self->buf, self->length + 1);
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
  return self->length;
}

/* ----- */

char
String_getChar (String *self, int i)
{
  return self->buf[i];
}
