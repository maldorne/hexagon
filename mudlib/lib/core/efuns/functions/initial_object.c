
static nomask object initial_object()
{
  object * list;
  list = previous_objects();

  if (!sizeof(list))
    return nil;

  return list[sizeof(list)-1];
}
