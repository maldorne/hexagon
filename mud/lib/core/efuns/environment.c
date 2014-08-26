

// Non-dgd efuns
// neverbot, 03/2014

// environment - return the environment of an object
// object environment( object ob );
// Return the containing object (environment) of 'ob'. If no argument
// is given, 'ob' defaults to this_object().  If the object is not
// inside anything, zero is returned.

// TODO environment

object environment( varargs object ob )
{
  if (!ob)
    ob = this_object();

  return nil;
}

// move_object - move current object to another environment
// void move_object( mixed dest );
// Move the current object into the object 'dest'.  dest should either be
// a filename or an object.

// TODO move_object

void move_object(mixed dest)
{

}

/*
 * Called if the environment is dested and there is no where else
 * to go...
 */
void destruct_environment_of(object ob) {
  object env;

  env = environment(ob);
  if (env) {
      string *a;
      a = (string *)env->query_dest_dir();
      if (catch(ob->move_player(a[0], a[1], "stumbles"))) {
    ob->move_player("void", "/room/void", "is sucked into the");
      }
  }
} /* destruct_environment_of() */
