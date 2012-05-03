#ifndef SYMBOL_H__
#define SYMBOL_H__

#define SYMBOL_NAME_MAX_SIZE 256

typedef struct Symbol Symbol;

Symbol *Symbol_new (const char name[SYMBOL_NAME_MAX_SIZE]);
void Symbol_delete (Symbol *self);

const char *Symbol_name (Symbol *self);

#endif /* SYMBOL_H__ */
