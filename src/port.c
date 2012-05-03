#include <stdio.h>
#include <stdarg.h>
#include "utils.h"
#include "port.h"

typedef enum {
  PFILE, PBUF
} PortType;

struct Port {
  PortType type;
  FILE *file;
  int pos, size;
  char *buf;
  char name[PORT_NAME_MAX_SIZE];
};

Port *
Port_newFile (FILE *file, const char name[PORT_NAME_MAX_SIZE])
{
  Port *self = NULL;

  alloc_one(self);

  self->type = PFILE;
  self->file = file;
  self->pos = -1;
  self->size = -1;
  self->buf = NULL;
  strncpy(self->name, name, PORT_NAME_MAX_SIZE);

  return self;
}

Port *
Port_newBuffer (char *buf, int size, const char name[PORT_NAME_MAX_SIZE])
{
  Port *self = NULL;

  alloc_one(self);

  self->type = PBUF;
  self->file = NULL;
  self->pos = 0;
  self->size = size + 1;
  alloc_(self->buf, self->size);
  strcpy(self->buf, buf);
  strncpy(self->name, name, PORT_NAME_MAX_SIZE);

  return self;
}

void Port_delete (Port *self)
{
  if (self == NULL) return;
  switch (self->type) {
  case PFILE:
    break;
  case PBUF:
    free_(self->buf);
    break;
  default:
    Utils_fatal("Port_delete: unknown port type.");
  }
  free_(self);
}

/* ----- */

char *
Port_name (Port *self)
{
  return self->name;
}

/* ----- */

int
Port_getc (Port *self)
{
  switch (self->type) {
  case PFILE:
    return getc(self->file);
  case PBUF:
    if (self->pos >= self->size)
      return EOF;
    {
      int c = self->buf[self->pos++];
      if (c == '\0') return EOF;
      return c;
    }
  default:
    Utils_fatal("Port_getc: unknown port type.");
  }

  return EOF;
}

int
Port_ungetc (Port *self, int c)
{
  switch (self->type) {
  case PFILE:
    return ungetc(c, self->file);
  case PBUF:
    if (self->pos <= 0)
      return EOF;
    return self->buf[--self->pos] = c;
  default:
    Utils_fatal("Port_ungetc: unknown port type.");
  }

  return EOF;
}

int
Port_printf (Port *self, const char *format, ...)
{
  va_list ap;
  va_start(ap, format);

  switch (self->type) {
  case PFILE:
    return vfprintf(self->file, format, ap);
  case PBUF:
    {
      int n = vsnprintf(self->buf + self->pos, self->size - self->pos,
                        format, ap);
      self->pos += n;
      return n;
    }
  default:
    Utils_fatal("Port_printf: unknown port type.");
  }

  return 0;
}

#define BUFFER_MAX_SIZE 2048

char *
Port_readline (Port *self)
{
  char c, *tmp = NULL, *s = NULL;

  alloc_(s, BUFFER_MAX_SIZE);

  tmp = s;

  while (tmp - s < BUFFER_MAX_SIZE &&
         (c = Port_getc(self)) != EOF && c != '\n')
    *tmp++ = c;

  return s;
}
