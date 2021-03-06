static Expression *
PrimitiveProc_symbol_to_string (Expression *args, Environment **env, Eval *ev)
{
  Expression *expr = NULL;

  nb_args("symbol->string", 1, args);

  if ((expr = Eval_eval(ev, car(args), env)) == NULL)
    return NULL;

  if (Expression_type(expr) != SYMBOL) {
    Utils_error("symbol->string: expected symbol");
    return NULL;
  }

  return Expression_new(STRING,
                        String_new(Symbol_name(Expression_expr(expr))));
}

static Expression *
PrimitiveProc_string_to_symbol (Expression *args, Environment **env, Eval *ev)
{
  Expression *expr = NULL;

  nb_args("string->symbol", 1, args);

  if ((expr = Eval_eval(ev, car(args), env)) == NULL)
    return NULL;

  if (Expression_type(expr) != STRING) {
    Utils_error("string->symbol: expected string");
    return NULL;
  }

  return Expression_new(SYMBOL, Symbol_new(String_buf(Expression_expr(expr))));
}

static Expression *
PrimitiveProc_string_to_number (Expression *args, Environment **env, Eval *ev)
{
  Expression *expr = NULL;

  nb_args("string->number", 1, args);

  if ((expr = Eval_eval(ev, car(args), env)) == NULL)
    return NULL;

  if (Expression_type(expr) != STRING) {
    Utils_error("string->number: expected string");
    return NULL;
  }

  return Expression_new(NUMBER,
                        Number_new(strtod(String_buf(Expression_expr(expr)),
                                          NULL)));
}

static Expression *
PrimitiveProc_number_to_string (Expression *args, Environment **env, Eval *ev)
{
  Expression *expr = NULL;
  char str[1024];

  nb_args("number->string", 1, args);

  if ((expr = Eval_eval(ev, car(args), env)) == NULL)
    return NULL;

  if (Expression_type(expr) != NUMBER) {
    Utils_error("number->string: expected number");
    return NULL;
  }

  snprintf(str, 1023, "%g", Number_val(Expression_expr(expr)));

  return Expression_new(STRING, String_new(str));
}

static Expression *
PrimitiveProc_string_to_list (Expression *args, Environment **env, Eval *ev)
{
  Expression *expr = NULL, *list = NULL;
  String *string = NULL;
  int i;

  nb_args("string->list", 1, args);

  if ((expr = Eval_eval(ev, car(args), env)) == NULL)
    return NULL;

  if (Expression_type(expr) != STRING) {
    Utils_error("string->list: expected string");
    return NULL;
  }

  string = Expression_expr(expr);
  list = Expression_new(NIL, NULL);
  for (i = String_length(string) - 1; i >= 0; i--)
    list = cons(Expression_new(CHARACTER,
                               Character_new(String_getChar(string, i))),
                list);

  return list;
}
