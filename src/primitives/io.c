static Expression *
PrimitiveProc_write (Expression *args, Environment **env, Eval *ev)
{
  Expression *expr = NULL;

  nb_args("write", 1, args);

  if ((expr = Eval_eval(ev, car(args), env)) == NULL)
    return NULL;

  Printer_print(Eval_printer(ev), expr);
  Port_printf(Eval_output(ev), "\n");

  return Expression_new(NIL, NULL);
}

static Expression *
PrimitiveProc_print (Expression *args, Environment **env, Eval *ev)
{
  Expression *expr = NULL;

  nb_args("print", 1, args);

  if ((expr = Eval_eval(ev, car(args), env)) == NULL)
    return NULL;

  if (Expression_type(expr) != STRING) {
    Utils_error("print: expected string");
    return NULL;
  }

  Port_printf(Eval_output(ev), "%s",
              String_buf(Expression_expr(expr)));

  return Expression_new(NIL, NULL);
}

static Expression *
PrimitiveProc_error (Expression *args, Environment **env, Eval *ev)
{
  Expression *expr = NULL;

  nb_args("error", 1, args);

  if ((expr = Eval_eval(ev, car(args), env)) == NULL)
    return NULL;

  if (Expression_type(expr) != STRING) {
    Utils_error("error: expected string");
    return NULL;
  }

  Port_printf(Eval_errput(ev), "Error: %s\n",
              String_buf(Expression_expr(expr)));

  return NULL;
}

static Expression *
PrimitiveProc_read (Expression *args, Environment **env, Eval *ev)
{
  nb_args("read", 0, args);

  return Parser_parseOneExpression(Eval_parser(ev));
}

static Expression *
PrimitiveProc_scan (Expression *args, Environment **env, Eval *ev)
{
  nb_args("scan", 0, args);

  return Expression_new(STRING, String_new(Port_readline(Eval_input(ev))));
}
