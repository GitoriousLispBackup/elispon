static Expression *
PrimitiveHelper_arith (char *name, Number *(*op)(Number *, Number*),
                       Expression *args, Environment **env, Eval *ev,
                       Number *neutral)
{
  Expression *list = args, *tmp = NULL;
  Number *result = neutral;

  while (Expression_type(list) != NIL) {
    if (Expression_type(list) != PAIR) {
      Utils_error("%s: expected proper argument list", name);
      return NULL;
    }

    if ((tmp = Eval_eval(ev, car(list), env)) == NULL)
      return NULL;

    if (Expression_type(tmp) != NUMBER) {
      Utils_error("%s: expected number", name);
      return NULL;
    }

    result = op(result, Expression_expr(tmp));
    list = cdr(list);
  }

  return Expression_new(NUMBER, result);
}

static Expression *
PrimitiveHelper_arith0 (char *name, Number *(*op)(Number *, Number*),
                        Expression *args, Environment **env, Eval *ev)
{
  Expression *expr = NULL;

  if (Expression_length(args) < 2)
    return PrimitiveHelper_arith(name, op, args, env, ev, Number_new(0));

  if ((expr = Eval_eval(ev, car(args), env)) == NULL)
    return NULL;

  if (Expression_type(expr) != NUMBER) {
    Utils_error("%s: expected number", name);
    return NULL;
  }

  return PrimitiveHelper_arith(name, op, cdr(args), env, ev,
                               Expression_expr(expr));
}

static Expression *
PrimitiveHelper_arith1 (char *name, Number *(*op)(Number *, Number*),
                        Expression *args, Environment **env, Eval *ev)
{
  Expression *expr = NULL;

  min_nb_args(name, 1, args);

  if (Expression_length(args) == 1)
    return PrimitiveHelper_arith(name, op, args, env, ev, Number_new(1));

  if ((expr = Eval_eval(ev, car(args), env)) == NULL)
    return NULL;

  if (Expression_type(expr) != NUMBER) {
    Utils_error("%s: expected number", name);
    return NULL;
  }

  return PrimitiveHelper_arith(name, op, cdr(args), env, ev,
                               Expression_expr(expr));
}

static Expression *
PrimitiveProc_add (Expression *args, Environment **env, Eval *ev)
{
  return PrimitiveHelper_arith("+", Number_add, args, env, ev, Number_new(0));
}

static Expression *
PrimitiveProc_sub (Expression *args, Environment **env, Eval *ev)
{
  return PrimitiveHelper_arith0("-", Number_sub, args, env, ev);
}

static Expression *
PrimitiveProc_mul (Expression *args, Environment **env, Eval *ev)
{
  return PrimitiveHelper_arith("*", Number_mul, args, env, ev, Number_new(1));
}

static Expression *
PrimitiveProc_div (Expression *args, Environment **env, Eval *ev)
{
  return PrimitiveHelper_arith1("/", Number_div, args, env, ev);
}

static Expression *
PrimitiveProc_idiv (Expression *args, Environment **env, Eval *ev)
{
  return PrimitiveHelper_arith1("div", Number_idiv, args, env, ev);
}

static Expression *
PrimitiveProc_mod (Expression *args, Environment **env, Eval *ev)
{
  return PrimitiveHelper_arith1("mod", Number_mod, args, env, ev);
}

static Expression *
PrimitiveProc_round (Expression *args, Environment **env, Eval *ev)
{
  Expression *expr = NULL;

  nb_args("round", 1, args);

  if ((expr = Eval_eval(ev, car(args), env)) == NULL)
    return NULL;

  if (Expression_type(expr) != NUMBER) {
    Utils_error("round: expected number");
    return NULL;
  }

  return Expression_new(NUMBER, Number_round(Expression_expr(expr)));
}

static Expression *
PrimitiveProc_abs (Expression *args, Environment **env, Eval *ev)
{
  Expression *expr = NULL;

  nb_args("abs", 1, args);

  if ((expr = Eval_eval(ev, car(args), env)) == NULL)
    return NULL;

  if (Expression_type(expr) != NUMBER) {
    Utils_error("abs: expected number");
    return NULL;
  }

  return Expression_new(NUMBER, Number_abs(Expression_expr(expr)));
}

static bool
PrimitiveHelper_arithp (char *name, bool (*pred)(Number *, Number*),
                        Expression *args, Environment **env, Eval *ev)
{
  Expression *expr = NULL, *prev = NULL;

  min_nb_args(name, 2, args);

  if ((prev = Eval_eval(ev, car(args), env)) == NULL)
    return NULL;

  if (Expression_type(prev) != NUMBER) {
    Utils_error("%s: expected number", name);
    return NULL;
  }

  args = cdr(args);

  while (Expression_type(args) != NIL) {
    if (Expression_type(args) != PAIR) {
      Utils_error("%s: expected proper argument list", name);
      return NULL;
    }

    if ((expr = Eval_eval(ev, car(args), env)) == NULL)
      return NULL;

    if (Expression_type(expr) != NUMBER) {
      Utils_error("%s: expected number", name);
      return NULL;
    }

    if (!pred(Expression_expr(prev), Expression_expr(expr)))
      return false;

    prev = expr;
    args = cdr(args);
  }

  return true;
}

static Expression *
PrimitiveProc_equal (Expression *args, Environment **env, Eval *ev)
{
  if (PrimitiveHelper_arithp("=", Number_eq, args, env, ev))
    return Expression_new(SYMBOL, Primitive_true());
  return Expression_new(NIL, NULL);
}

static Expression *
PrimitiveProc_lesser (Expression *args, Environment **env, Eval *ev)
{
  if (PrimitiveHelper_arithp("<", Number_lt, args, env, ev))
    return Expression_new(SYMBOL, Primitive_true());
  return Expression_new(NIL, NULL);
}

static Expression *
PrimitiveProc_greater (Expression *args, Environment **env, Eval *ev)
{
  if (PrimitiveHelper_arithp(">", Number_gt, args, env, ev))
    return Expression_new(SYMBOL, Primitive_true());
  return Expression_new(NIL, NULL);
}
