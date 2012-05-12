static Expression *
PrimitiveProc_cons (Expression *args, Environment **env, Eval *ev)
{
  Expression *fst = NULL, *snd = NULL;

  nb_args("cons", 2, args);

  if ((fst = Eval_eval(ev, car(args), env)) == NULL)
    return NULL;
  if ((snd = Eval_eval(ev, cadr(args), env)) == NULL)
    return NULL;

  return cons(fst, snd);
}

static Expression *
PrimitiveProc_car (Expression *args, Environment **env, Eval *ev)
{
  Expression *expr = NULL;

  nb_args("car", 1, args);

  if ((expr = Eval_eval(ev, car(args), env)) == NULL)
    return NULL;

  if (Expression_type(expr) != PAIR) {
    Utils_error("car: expected pair");
    return NULL;
  }

  return car(expr);
}

static Expression *
PrimitiveProc_cdr (Expression *args, Environment **env, Eval *ev)
{
  Expression *expr = NULL;

  nb_args("cdr", 1, args);

  if ((expr = Eval_eval(ev, car(args), env)) == NULL)
    return NULL;

  if (Expression_type(expr) != PAIR) {
    Utils_error("cdr: expected pair");
    return NULL;
  }

  return cdr(expr);
}

static Expression *
PrimitiveProc_list (Expression *args, Environment **env, Eval *ev)
{
  Expression *list = NULL, *expr = NULL, *tmp = NULL, *prev = NULL;

  list = tmp = Expression_new(PAIR, Pair_new(NULL, NULL));
  while (Expression_type(args) == PAIR) {
    if ((expr = Eval_eval(ev, car(args), env)) == NULL)
      return NULL;

    Expression_setCar(tmp, expr);
    Expression_setCdr(tmp, Expression_new(PAIR, Pair_new(NULL, NULL)));
    prev = tmp;
    tmp = cdr(tmp);
    args = cdr(args);
  }

  if (Expression_type(args) != NIL) {
    if ((expr = Eval_eval(ev, args, env)) == NULL)
      return NULL;

    Expression_setCdr(prev, expr);
  }
  else {
    Expression_setExpr(tmp, NULL);
    Expression_setType(tmp, NIL);
  }

  return list;
}
