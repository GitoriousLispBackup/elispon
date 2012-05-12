static Expression *
PrimitiveHelper_typep (char *name, ExprType type,
                       Expression *args, Environment **env, Eval *ev)
{
  Expression *expr = NULL;

  nb_args(name, 1, args);

  if ((expr = Eval_eval(ev, car(args), env)) == NULL)
    return NULL;

  if (Expression_type(expr) == type)
    return Expression_new(SYMBOL, Primitive_true());
  return Expression_new(NIL, NULL);
}

static Expression *
PrimitiveProc_primitivep (Expression *args, Environment **env, Eval *ev)
{
  return PrimitiveHelper_typep("primitive?", PRIMITIVE, args, env, ev);
}

static Expression *
PrimitiveProc_nullp (Expression *args, Environment **env, Eval *ev)
{
  return PrimitiveHelper_typep("null?", NIL, args, env, ev);
}

static Expression *
PrimitiveProc_pairp (Expression *args, Environment **env, Eval *ev)
{
  return PrimitiveHelper_typep("pair?", PAIR, args, env, ev);
}

static Expression *
PrimitiveProc_symbolp (Expression *args, Environment **env, Eval *ev)
{
  return PrimitiveHelper_typep("symbol?", SYMBOL, args, env, ev);
}

static Expression *
PrimitiveProc_characterp (Expression *args, Environment **env, Eval *ev)
{
  return PrimitiveHelper_typep("character?", CHARACTER, args, env, ev);
}

static Expression *
PrimitiveProc_stringp (Expression *args, Environment **env, Eval *ev)
{
  return PrimitiveHelper_typep("string?", STRING, args, env, ev);
}

static Expression *
PrimitiveProc_numberp (Expression *args, Environment **env, Eval *ev)
{
  return PrimitiveHelper_typep("number?", NUMBER, args, env, ev);
}

static Expression *
PrimitiveProc_fexprp (Expression *args, Environment **env, Eval *ev)
{
  return PrimitiveHelper_typep("fexpr?", FEXPR, args, env, ev);
}

static Expression *
PrimitiveProc_vectorp (Expression *args, Environment **env, Eval *ev)
{
  return PrimitiveHelper_typep("vector?", VECTOR, args, env, ev);
}

static Expression *
PrimitiveProc_environmentp (Expression *args, Environment **env, Eval *ev)
{
  return PrimitiveHelper_typep("environment?", ENVIRONMENT, args, env, ev);
}

static Expression *
PrimitiveProc_structp (Expression *args, Environment **env, Eval *ev)
{
  return PrimitiveHelper_typep("struct?", STRUCT, args, env, ev);
}

static Expression *
PrimitiveProc_objectp (Expression *args, Environment **env, Eval *ev)
{
  return PrimitiveHelper_typep("object?", OBJECT, args, env, ev);
}
