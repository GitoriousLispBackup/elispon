#ifndef PAIR_H__
#define PAIR_H__

#include "expression.h"

typedef struct Pair Pair;

Pair *Pair_new (Expression *fst, Expression *snd);
void Pair_delete (Pair *self);

Expression *Pair_fst (Pair *self);
Expression *Pair_snd (Pair *self);


#endif /* PAIR_H__ */
