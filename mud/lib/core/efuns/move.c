
// Non-dgd efuns and stuff
// neverbot, 03/2014
// init added, 12/2014

#include <basic/move.h>

private static object _environment;
private static object * _inventory;


object * all_inventory(varargs object ob);
object * deep_inventory(varargs object ob);

// void init();
// When the mudlib moves an object "A" inside another object "B", the
// driver (the move_object() efunction) does the following:

// * if "A" is living, causes "A" to call the init() in "B"
// * causes each living object in the inventory of "B" to call init() in
// "A".  regardless of whether "A" is living or not.
// * if "A" is living, causes "A" to call the init() in each object in
// the inventory of "B".

// Note: an object is considered to be living if enable_commands() has
// been called by that object.

// Typically, the init() function in an object is used to call add_action()
// for each command that the object offers.

void init()
{

}

// old definitions
// int test_add(object ob, int flag) { return !flag; }
// int test_remove(object ob, int flag) { return !flag; }

// can add object ob to our inventory?
int test_add(object ob)
{
  return 1;
}

// can remove object ob from our inventory?
int test_remove(object ob)
{
  // if item not in inventory
  if (member_array(ob, _inventory) == -1)
    return -1;

  return 1;
}

// inner functions that simulates the mudos inventory
// and environment handling
// these should only be called from _move
// NEVER call this _functions from other places
// in the mudlib

nomask int _inv_add(object ob)
{
  // item already in inventory
  if (member_array(ob, _inventory) != -1)
    return -1;

  _inventory += ({ ob });

  return 1;
}

nomask int _inv_remove(object ob)
{
  // if item not in inventory
  if (member_array(ob, _inventory) == -1)
    return -1;

  _inventory -= ({ ob });

  return 1;
}

nomask int _move(varargs object dest)
{
  // if we are doing something like ob->move(),
  // then we are trying to move inside the caller
  // object
  if (!dest)
    dest = previous_object();

  if (!dest)
    return MOVE_LIB_NO_DEST;

  // moving inside ourselves
  if (dest == this_object())
    return MOVE_LIB_INSIDE;

  // avoid loops
  if (member_array(dest, deep_inventory()) != -1)
    return MOVE_LIB_LOOP;

  // update inventories and current environment
  if (_environment)
    _environment->_inv_remove(this_object());
  
  _environment = dest;

  if (_environment)
    _environment->_inv_add(this_object());

  // TODO
  // init calling

  return MOVE_OK;
}


// environment - return the environment of an object
// object environment( object ob );
// Return the containing object (environment) of 'ob'. If no argument
// is given, 'ob' defaults to this_object().  If the object is not
// inside anything, zero is returned.

nomask object environment( varargs object ob )
{
  if (!ob)
    ob = this_object();

  return _environment;
}

// move_object - move current object to another environment
// void move_object( mixed dest );
// Move the current object into the object 'dest'.  dest should either be
// a filename or an object.

void move_object(mixed dest)
{
  // if the move function exists in this object, move
  if (function_object("move", this_object()))
    this_object()->move(dest);
}

// Called if the environment is dested and there is no where else
// to go...

void destruct_environment_of(object ob) 
{
  object env;

  env = environment(ob);
  if (env) 
  {
      string *a;
      a = (string *)env->query_dest_dir();

      if (catch(ob->move_player(a[0], a[1], "stumbles"))) 
      {
        ob->move_player("void", "/room/void", "is sucked into the");
      }
  }
} 

// object array all_inventory( object ob );
// Returns an array of the objects contained inside of 'ob'.  If 'ob' is
// omitted, this_object() is used.

object * all_inventory(varargs object ob)
{
  if (!ob)
    ob = this_object();

  return _inventory;
}

// object *deep_inventory( object ob );
// Returns an array of the objects contained in the inventory
// of <ob> and all the objects contained in the inventories of
// those objects and so on. The return array of objects is flat,
// i.e. there is no structure reflecting internal containment
// relations.

object * deep_inventory(varargs object ob)
{
  object * result;
  object * pending;
  int i;

  if (!ob)
    ob = this_object();

  pending = all_inventory(ob);

  // recursively check the inventory of all
  // the items
  while(sizeof(pending) > 0)
  {
    object current;
    current = pending[0];

    result += ({ current });
    pending -= ({ current });
    pending += all_inventory(current);
  }

  return result;
}

// object first_inventory( mixed ob );
// Return the first object in the inventory of 'ob', where 'ob' is
// either an object or the file name of an object.

object first_inventory(mixed ob)
{
  object tmp;
  object * list;

  // load the object if the parameter is a string
  if (stringp(ob))
  {
    ob = load_object(ob);
    if (!ob)
      return nil;
  }

  list = ob->all_inventory();

  if (sizeof(list))
    return list[0];

  return nil;
}

mixed query_strange_inventory(mixed arr) 
{
  mixed inv;
  object ob;
  mixed desc;
  int i,k;

  inv = ({ });
  for (k = 0; k < sizeof(arr); k++) 
  {
    ob = arr[k];

    if (!(desc = (string)ob->short()) || (desc == ""))
      continue;

    if ((i = member_array(desc, inv)) >= 0)
      inv[i+1] += ({ ob });
    else
      inv += ({ desc, ({ ob }) });
  }

  return inv;
}

