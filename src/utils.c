#include <stdio.h>
#include <stdarg.h>
#include "expression.h"
#include "symbol.h"
#include "utils.h"

FILE *
Utils_openFile (const char *path, const char *mode)
{
  FILE *f = NULL;

  if ((f = fopen(path, mode)) == NULL)
    Utils_fatal("fopen(%s, %s): %s", path, mode, strerror(errno));

  return f;
}

void
Utils_closeFile (FILE *f)
{
  fclose(f);
}

/* ----- */

void
Utils_error (const char *format, ...)
{
  va_list ap;
  va_start(ap, format);

  fprintf(stderr, "Error: ");
  vfprintf(stderr, format, ap);
  fprintf(stderr, "\n");
}

void
Utils_fatal (const char *format, ...)
{
  va_list ap;
  va_start(ap, format);

  fprintf(stderr, "Fatal error: ");
  vfprintf(stderr, format, ap);
  fprintf(stderr, "\n");

  exit(EXIT_FAILURE);
}

/* ----- */

Symbol *
Utils_findSymbol (Expression *symbols, char *name)
{
  Symbol *sym = NULL;

  while (Expression_type(symbols) != NIL) {
    sym = Expression_expr(Expression_car(symbols));
    if (strcmp(name, Symbol_name(sym)) == 0)
      return sym;
    symbols = Expression_cdr(symbols);
  }

  return NULL;
}
