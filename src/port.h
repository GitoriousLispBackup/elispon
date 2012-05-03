#ifndef PORT_H__
#define PORT_H__

#include <stdarg.h>

#define PORT_NAME_MAX_SIZE 512

typedef struct Port Port;

Port *Port_newFile (FILE *file, const char name[PORT_NAME_MAX_SIZE]);
Port *Port_newBuffer (char *buf, int size, const char *name);
void Port_delete (Port *self);

char *Port_name (Port *self);

int Port_getc (Port *self);
int Port_ungetc (Port *self, int c);
int Port_printf (Port *self, const char *format, ...);
char *Port_readline (Port *self);

#endif /* PORT_H__ */
