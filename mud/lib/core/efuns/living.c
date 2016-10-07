
// Non-dgd stuff
// neverbot, 12/2014

#include <living/living.h>

private static int _living;

nomask int query_living() { return _living; }

// int living( object ob );
// Return true if `ob' is a living object (that is, if "enable_commands()" has
// been called by `ob').

static nomask int living(object ob)
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

nomask void enable_commands()
{
  // only this_object can do this
  if (!previous_object() || 
      (previous_object() != this_object())) 
    return;

  _living = 1;
}


// Find first the object that is marked as living, and answers to the
// id 'str'.  A living object is an object that has called
// enable_commands().  The object must have set a name with
// set_living_name(), so its name will be entered into the hash table
// used to speed up the search for living objects.

static nomask object find_living(string name)
{
  return find_object(LIVING_HANDLER)->_find_living(name);
}

// void set_living_name( string name );
// Set a living name on an object that is living. After this has been done, the
// object can be found with "find_living()".

nomask void set_living_name(string name)
{
  find_object(LIVING_HANDLER)->_set_living_name(this_object(), name);
}
