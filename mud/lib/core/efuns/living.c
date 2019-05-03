
#include <kernel.h>
#include <living/living.h>
#include <user/user.h>

#include "livings/living.c"

// void enable_commands();
// enable_commands() marks this_object() as a living object, and allows
// it to use commands added with add_action() (by using command()).
// When enable_commands() is called, the driver also looks for the
// local function catch_tell(), and if found, it will call it every time
// a message (via say() for example) is given to the object.

nomask void enable_commands()
{
  // TODO check npcs too
  if (base_name(previous_object()) != LOGIN_OB)
    return;

  _living = 1;
}

// livings - return an array of all living objects
// object array livings();
// Returns an array of pointers to all living objects (objects that have
// had enable_commands() called in them).

static nomask object * livings()
{
  return find_object(LIVING_HANDLER)->query_livings();
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
  // TODO if interactive, only login can do this
  find_object(LIVING_HANDLER)->_set_living_name(this_object(), name);
}
