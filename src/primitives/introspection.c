static Expression *
PrimitiveProc_open_fexpr (Expression *args, Environment **env, Eval *ev)
{
  Expression *expr = NULL;
  Fexpr *f = NULL;

  nb_args("%open-fexpr%", 1, args);

  if ((expr = Eval_eval(ev, car(args), env)) == NULL)
    return NULL;

  if (Expression_type(expr) != FEXPR) {
    Utils_error("%%open-fexpr%%: expected fexpr");
    return NULL;
  }

  f = Expression_expr(expr);

  return cons(cons(Expression_new(SYMBOL, Fexpr_arg(f)),
                   cons(Expression_new(SYMBOL, Fexpr_dynenv(f)),
                        cons(Fexpr_body(f), Expression_new(NIL, NULL)))),
              Expression_new(ENVIRONMENT, Fexpr_lexenv(f)));
}

static Expression *
PrimitiveProc_open_struct (Expression *args, Environment **env, Eval *ev)
{
  Expression *expr = NULL;
  Symbol **members = NULL;
  int i, size;

  nb_args("%open-struct%", 1, args);

  if ((expr = Eval_eval(ev, car(args), env)) == NULL)
    return NULL;

  if (Expression_type(expr) != STRUCT) {
    Utils_error("%%open-struct%%: expected struct");
    return NULL;
  }

  size = Struct_size(Expression_expr(expr));
  members = Struct_members(Expression_expr(expr));

  expr = Expression_new(NIL, NULL);
  for (i = size - 1; i >= 0; i--)
    expr = cons(Expression_new(SYMBOL, members[i]), expr);

  return expr;
}
