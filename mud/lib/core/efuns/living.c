
// Non-dgd stuff
// neverbot, 12/2014

static int _living;

nomask int query_living() { return _living; }

// int living( object ob );
// Return true if `ob' is a living object (that is, if "enable_commands()" has
// been called by `ob').

nomask int living(object ob)
{
  if (ob)
    return ob->query_living();
  
  return _living;
}

// void enable_commands();
// enable_commands() marks this_object() as a living object, and allows
// it to use commands added with add_action() (by using command()).
// When enable_commands() is called, the driver also looks for the
// local function catch_tell(), and if found, it will call it every time
// a message (via say() for example) is given to the object.

nomask enable_commands()
{
  // no needed, private function
  // only this_object can do this
  // if (!previous_object() || previous_object() != this_object()) 
  //   return;

  _living = 1;
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
