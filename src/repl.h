#ifndef REPL_H__
#define REPL_H__

typedef struct REPL REPL;

REPL *REPL_new ();
void REPL_delete (REPL *self);

REPL *REPL_read (REPL *self);
REPL *REPL_eval (REPL *self);
REPL *REPL_print (REPL *self);

#endif /* REPL_H__ */
