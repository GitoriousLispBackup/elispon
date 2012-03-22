#include <stdio.h>
#include "repl.h"

int
main (int argc, char *argv[])
{
  REPL *repl = NULL;

  repl = REPL_new();

  while (REPL_print(REPL_eval(REPL_read(repl))));

  REPL_delete(repl);

  return 0;
}
