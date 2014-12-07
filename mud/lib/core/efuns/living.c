
// Non-dgd stuff
// neverbot, 12/2014

int living(object ob)
{
  return 1;
}

// Find first the object that is marked as living, and answers to the
// id 'str'.  A living object is an object that has called
// enable_commands().  The object must have set a name with
// set_living_name(), so its name will be entered into the hash table
// used to speed up the search for living objects.

// TODO find_living

object find_living(string name)
{
  // string nick;
  // object ob;

  // if (this_player() && (nick = (string)this_player()->expand_nickname(str)))
  //   str = nick;

  // if ((ob = efun::find_living(str)))
  //   if ((int)ob->query_invis() == 2)
  //     return nil;

  // return ob;

  return nil;
}
