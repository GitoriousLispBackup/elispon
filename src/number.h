#ifndef NUMBER_H__
#define NUMBER_H__

typedef struct Number Number;

Number *Number_new (double val);
void Number_delete (Number *self);

double Number_val (Number *self);

Number *Number_add (Number *self, Number *num);
Number *Number_sub (Number *self, Number *num);
Number *Number_mul (Number *self, Number *num);
Number *Number_div (Number *self, Number *num);
Number *Number_idiv (Number *self, Number *num);
Number *Number_mod (Number *self, Number *num);
bool Number_eq (Number *self, Number *num);
bool Number_lt (Number *self, Number *num);
bool Number_gt (Number *self, Number *num);

#endif /* NUMBER_H__ */
