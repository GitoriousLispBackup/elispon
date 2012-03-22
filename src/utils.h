#ifndef UTILS_H__
#define UTILS_H__

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "expression.h"
#include "symbol.h"

#define alloc_one(ptr) alloc_(ptr, 1)

#define alloc_(ptr, size)                               \
  (((ptr = malloc(size * sizeof(*(ptr)))) == NULL)      \
   ? (Utils_fatal("malloc: %s", strerror(errno)), NULL) \
   : ptr)

#define free_(ptr) (free(ptr), ptr = NULL)

#ifdef DEBUG 
#define debug_in() Utils_log("Debug: entering %s", __func__);
#define debug_out() Utils_log("Debug: leaving %s", __func__);
#else
#define debug_in()
#define debug_out()
#endif

FILE *Utils_openFile (const char *path, const char *mode);
void Utils_closeFile (FILE *f);

void Utils_error (const char *format, ...);
void Utils_fatal (const char *format, ...);

Symbol *Utils_findSymbol (Expression *symbols, char *name);

#endif /* UTILS_H__ */
