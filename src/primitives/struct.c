static Expression *
PrimitiveProc_struct (Expression *args, Environment **env, Eval *ev)
{
  Struct *structure = NULL;
  Expression *member = NULL;
  int i = 0, size;

  size = Expression_length(args);

  structure = Struct_new(size);

  while (Expression_type(args) != NIL) {
    if (Expression_type(args) != PAIR) {
      Utils_error("struct: expected proper list");
      return NULL;
    }

    if (Expression_type(member = car(args)) != SYMBOL) {
      Utils_error("struct: members name must be symbol");
      return NULL;
    }

    Struct_declareMember(structure, i++, Expression_expr(member));

    args = cdr(args);
  }

  return Expression_new(STRUCT, structure);
}

static Expression *
PrimitiveProc_type (Expression *args, Environment **env, Eval *ev)
{
  Expression *expr = NULL;

  nb_args("type", 1, args);

  if ((expr = Eval_eval(ev, car(args), env)) == NULL)
    return NULL;

  if (Expression_type(expr) != OBJECT) {
    Utils_error("type: expected object has argument");
    return NULL;
  }

  return Expression_new(STRUCT, Object_type(Expression_expr(expr)));
}
