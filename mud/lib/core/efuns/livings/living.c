

private static int _living;

nomask int query_living() { return _living; }

// living - detects whether or not a given object is "living"
// int living( object ob );
// Return true if `ob' is a living object (that is, if "enable_commands()" has
// been called by `ob').

static nomask int living(varargs object ob)
{
  if (ob)
    return ob->query_living();
  
  return _living;
}
