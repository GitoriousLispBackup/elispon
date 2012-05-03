#ifndef SYMBOL_TABLE_H__
#define SYMBOL_TABLE_H__

#include "symbol.h"

typedef struct SymbolTable SymbolTable;

SymbolTable *SymbolTable_new (char **primitives);
void SymbolTable_delete (SymbolTable *self);

Symbol *SymbolTable_find (SymbolTable *self, const char *name);

#endif /* SYMBOL_TABLE_H__ */
