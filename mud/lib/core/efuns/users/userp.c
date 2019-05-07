
// userp - determine if a given object was once interactive
// int userp( object );
// Returns 1 if the arg was once interactive.

static int userp(object ob)
{
  return interactive(ob);
}
