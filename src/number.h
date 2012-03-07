#ifndef NUMBER_H__
#define NUMBER_H__

typedef struct Number Number;

Number *Number_new (double val);
void Number_delete (Number *self);

double Number_val (Number *self);

#endif /* NUMBER_H__ */
