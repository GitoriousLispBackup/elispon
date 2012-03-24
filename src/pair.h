#ifndef PAIR_H__
#define PAIR_H__

#include "expression.h"

typedef struct Pair Pair;

Pair *Pair_new (Expression *fst, Expression *snd);
void Pair_delete (Pair *self);

Expression *Pair_fst (Pair *self);
Expression *Pair_snd (Pair *self);
void Pair_setFst (Pair *self, Expression *expr);
void Pair_setSnd (Pair *self, Expression *expr);
bool Pair_flag (Pair *self);
void Pair_setFlag (Pair *self, bool flag);

#endif /* PAIR_H__ */
