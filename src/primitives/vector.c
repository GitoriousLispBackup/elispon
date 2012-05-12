static Expression *
PrimitiveProc_vector (Expression *args, Environment **env, Eval *ev)
{
  Expression *expr = NULL;
  Vector *vector = NULL;
  int i = 0, size;

  size = Expression_length(args);
  vector = Vector_new(size);

  while (Expression_type(args) != NIL) {
    if ((expr = Eval_eval(ev, car(args), env)) == NULL)
      return NULL;

    Vector_set(vector, i++, expr);

    args = cdr(args);
  }

  return Expression_new(VECTOR, vector);
}
