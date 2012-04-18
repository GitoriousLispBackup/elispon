#ifndef STRUCT_H__
#define STRUCT_H__

#include "symbol.h"

typedef struct Struct Struct;

Struct *Struct_new (int size);
void Struct_delete (Struct *self);

int Struct_size (Struct *self);
Symbol **Struct_members (Struct *self);

void Struct_declareMember (Struct *self, int position, Symbol *member);
int Struct_memberPosition (Struct *self, Symbol *member);

#endif /* STRUCT_H__ */
