static Expression *
PrimitiveProc_define (Expression *args, Environment **env, Eval *ev)
{
  Expression *expr = NULL, *tmp = NULL;

  nb_args("define", 2, args);

  if (Expression_type(car(args)) != SYMBOL) {
    Utils_error("define: expected symbol as first argument");
    return NULL;
  }

  if (Environment_find(*env, Expression_expr(car(args))) != NULL) {
    Utils_error("define: %s: already defined symbol",
                Symbol_name(Expression_expr(car(args))));
    return NULL;
  }

  tmp = Expression_new(NIL, NULL);
  Environment_add(*env, Expression_expr(car(args)), tmp);

  if ((expr = Eval_eval(ev, cadr(args), env)) == NULL) {
    Environment_pop(*env);
    return NULL;
  }

  Expression_setType(tmp, Expression_type(expr));
  Expression_setExpr(tmp, Expression_expr(expr));

  return car(args);
}

static Expression *
PrimitiveProc_set (Expression *args, Environment **env, Eval *ev)
{
  Expression *expr = NULL, *tmp = NULL;

  nb_args("set", 2, args);

  if (Expression_type(car(args)) != SYMBOL) {
    Utils_error("set: expected symbol as first argument");
    return NULL;
  }

  if (Environment_find(*env, Expression_expr(car(args))) == NULL) {
    Utils_error("set: %s: inexistant symbol",
                Symbol_name(Expression_expr(car(args))));
    return NULL;
  }

  tmp = Expression_new(NIL, NULL);
  Environment_add(*env, Expression_expr(car(args)), tmp);

  if ((expr = Eval_eval(ev, cadr(args), env)) == NULL)
    return NULL;

  Expression_setType(tmp, Expression_type(expr));
  Expression_setExpr(tmp, Expression_expr(expr));

  return car(args);
}

static Expression *
PrimitiveProc_sequence (Expression *args, Environment **env, Eval *ev)
{
  Expression *expr = NULL;

  while (Expression_type(args) != NIL) {
    if (Expression_type(args) != PAIR) {
      Utils_error("sequence: expected proper list");
      return NULL;
    }
    if ((expr = Eval_eval(ev, car(args), env)) == NULL)
      return NULL;

    args = cdr(args);
  }

  return expr;
}

static Expression *
PrimitiveProc_if (Expression *args, Environment **env, Eval *ev)
{
  Expression *cond = NULL;

  nb_args("if", 3, args);

  if ((cond = Eval_eval(ev, car(args), env)) == NULL)
    return NULL;

  if (Expression_type(cond) != NIL)
    return Eval_eval(ev, cadr(args), env);
  return Eval_eval(ev, caddr(args), env);
}

static Expression *
PrimitiveProc_same (Expression *args, Environment **env, Eval *ev)
{
  Expression *expr1 = NULL, *expr2;

  nb_args("same?", 2, args);

  if ((expr1 = Eval_eval(ev, car(args), env)) == NULL)
    return NULL;

  if ((expr2 = Eval_eval(ev, cadr(args), env)) == NULL)
    return NULL;

  if (Expression_expr(expr1) == Expression_expr(expr2))
    return Expression_new(SYMBOL, Primitive_true());
  return Expression_new(NIL, NULL);
}

static Expression *
PrimitiveProc_eval (Expression *args, Environment **env, Eval *ev)
{
  Expression *expr = NULL, *environment = NULL;
  Environment *environ = NULL;
  int nb_args;

  minmax_nb_args("eval", 1, 2, args, &nb_args);

  if ((expr = Eval_eval(ev, car(args), env)) == NULL)
    return NULL;

  if (nb_args == 2) {
    if ((environment = Eval_eval(ev, cadr(args), env)) == NULL)
      return NULL;

    environ = Expression_expr(environment);
    return Eval_eval(ev, expr, &environ);
  }

  return Eval_eval(ev, expr, env);
}

static Expression *
PrimitiveProc_vau (Expression *args, Environment **env, Eval *ev)
{
  nb_args("vau", 3, args);

  if (Expression_type(car(args)) != SYMBOL ||
      Expression_type(cadr(args)) != SYMBOL ||
      Expression_expr(car(args)) == Expression_expr(cadr(args))) {
    Utils_error("vau: expected the first two arguments to be different"
                " symbols");
    return NULL;
  }

  return Expression_new(FEXPR, Fexpr_new(Expression_expr(car(args)),
                                         Expression_expr(cadr(args)),
                                         caddr(args),
                                         Environment_copy(*env)));
}

static Expression *
PrimitiveProc_environment (Expression *args, Environment **env, Eval *ev)
{
  nb_args("environment", 0, args);

  return Expression_new(ENVIRONMENT, Environment_copy(*env));
}

static Expression *
PrimitiveProc_length (Expression *args, Environment **env, Eval *ev)
{
  Expression *expr = NULL;
  int length;

  nb_args("length", 1, args);

  if ((expr = Eval_eval(ev, car(args), env)) == NULL)
    return NULL;

  switch (Expression_type(expr)) {
  case PAIR:
    length = Expression_length(expr);
    break;
  case STRING:
    length = String_length(Expression_expr(expr));
    break;
  case VECTOR:
    length = Vector_size(Expression_expr(expr));
    break;
  default:
    length = 0;
  }

  return Expression_new(NUMBER, Number_new(length));
}
