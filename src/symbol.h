#ifndef SYMBOL_H__
#define SYMBOL_H__

typedef struct Symbol Symbol;

#define MAX_SYMBOL_LENGTH 256

Symbol *Symbol_new (char name[MAX_SYMBOL_LENGTH]);
void Symbol_delete (Symbol *self);

char *Symbol_name (Symbol *self);

#endif /* SYMBOL_H__ */
