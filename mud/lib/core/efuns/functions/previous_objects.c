
static nomask object * previous_objects(varargs int step)
{
  object result;
  int arg;

  if (nullp(step))
    arg = 0;
  else
    arg = step;

  result = previous_object(arg);

  if (result == nil)
    return ({ });

  return ({ result }) + previous_objects(arg + 1);
}
