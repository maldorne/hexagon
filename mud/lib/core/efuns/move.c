
#include <basic/move.h>
#include <areas/common.h>

private static object _environment;
private static object * _inventory;


nomask object * all_inventory(varargs object ob);
nomask object * deep_inventory(varargs object ob);

// can add object ob to our inventory?
int test_add(object ob, int flag)
{
  return !flag;
}

// can remove object ob from our inventory?
int test_remove(object ob, int flag)
{
  // if item not in inventory
  if (member_array(ob, _inventory) == -1)
    return -1;

  return !flag;
}

// inner functions that simulates the mudos inventory
// and environment handling
// these should only be called from _move
// NEVER call this _functions from other places
// in the mudlib

nomask int _inv_add(object ob)
{
  if (!arrayp(_inventory))
    _inventory = ({ });

  // item already in inventory
  if (member_array(ob, _inventory) != -1)
    return -1;

  _inventory += ({ ob });

  return 1;
}

nomask int _inv_remove(object ob)
{
  if (!arrayp(_inventory))
    _inventory = ({ });

  // if item not in inventory
  if (member_array(ob, _inventory) == -1)
    return -1;

  _inventory -= ({ ob });

  return 1;
}

int move(varargs object dest)
{
  object * contents;
  object old_this_player, old_this_user;
  int i;

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

  stderr(" * move <" + object_name(this_object()) +
         (_environment ? ("> from <" + object_name(_environment)) : "") +
         "> to <" + object_name(dest) + "> " + (living(this_object()) ? "(living)" : "") + "\n");

  // update inventories and current environment
  if (_environment)
    _environment->_inv_remove(this_object());

  _environment = dest;

  if (_environment)
    _environment->_inv_add(this_object());

  // init calling

  // save this_player to restore it afterwards
  old_this_player = this_player();
  // old_this_user = this_user();

  // get all objects in the new environment
  contents = all_inventory(_environment);

  if (living(this_object()))
  {
    stderr(" ~~~ move of a living\n");
    MUDOS->set_initiator_object(this_object());

    // call init in the environment
    catch(call_other(_environment, "init"));

    // call init in the environment contents
    for (i = 0; i < sizeof(contents); i++)
      catch(call_other(contents[i], "init"));
  }
  // moving something to a player inventory
  else if (living(_environment))
  {
    MUDOS->do_init(_environment, this_object());
  }

  stderr("   - M before, this_object():\n    "  + to_string(this_object()));
  stderr("   - M before, this_player():\n    "  + to_string(this_player()));
  stderr("   - M before, this_player(1):\n    " + to_string(this_player(1)));
  stderr("   - M before, this_user():\n    "    + to_string(this_user()));

  // call out own init() from all living objects in dest
  map_array(filter_array(contents, "living", MUDOS),
            "do_init", MUDOS, old_this_player);

  if (living(this_object()))
  {
    stderr(" ~~~ end move of a living\n");
    MUDOS->set_initiator_object(old_this_player);
  }

  return MOVE_OK;
}

// environment - return the environment of an object
// object environment( object ob );
// Return the containing object (environment) of 'ob'. If no argument
// is given, 'ob' defaults to this_object().  If the object is not
// inside anything, zero is returned.

nomask object environment(varargs object ob)
{
  if (ob)
    return ob->environment();

  return _environment;
}

// move_object - move current object to another environment
// void move_object( mixed dest );
// Move the current object into the object 'dest'.  dest should either be
// a filename or an object.

void move_object(mixed dest)
{
  // if the move function exists in this object, move
  // if (function_object("move", this_object()))
  //   this_object()->move(dest);
  move(dest);
}

// destruct - remove an object
// void destruct( object ob );

// Completely destroy and remove object 'ob'. After the call to destruct().
// If 'ob' is this_object(), execution will continue, but it is best to return
// a value immediately.  All pointers to the object in any variable or structure
// will immediately become zero.  move_or_destruct() is called in all the
// objects inside of the object being destructed.

nomask int destruct(varargs object ob)
{
  object env;
  object * shadows;
  int i;

  stderr(" - destruct: <" + object_name(ob) + ">\n");

  if (!ob)
    ob = this_object();

  // destruct all objects shadowing this object
  shadows = ob->_query_shadows();

  for (i = 0; i < sizeof(shadows); i++)
    destruct(shadows[i]);

  // TODO destruct its inventory?

  env = environment(ob);

  if (env)
  {
    if (!env->_inv_remove(ob))
    {
      stderr(" *** destruct: <" + object_name(ob) + "> error, its environment refused to let it go\n");
      return 0;
    }

    _environment = nil;
  }

  ::destruct_object(ob);

  return 1;
  // return (this_object() == nil);
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

      if (catch(ob->move_living(a[0], a[1], "stumbles")))
      {
        ob->move_living("void", ROOM_VOID, "is sucked into the");
      }
  }
}

// object array all_inventory( object ob );
// Returns an array of the objects contained inside of 'ob'.  If 'ob' is
// omitted, this_object() is used.

nomask object * all_inventory(varargs object ob)
{
  if (ob)
    return ob->all_inventory();

  if (arrayp(_inventory))
  {
    // _inventory -= ({ 0 });
    // return array_copy(_inventory);
    return _inventory;
  }
  else
  {
    return ({ });
  }
}

// object *deep_inventory( object ob );
// Returns an array of the objects contained in the inventory
// of <ob> and all the objects contained in the inventories of
// those objects and so on. The return array of objects is flat,
// i.e. there is no structure reflecting internal containment
// relations.

nomask object * deep_inventory(varargs object ob)
{
  object * result;
  object * pending;
  int i;

  result = ({ });

  if (!ob)
    ob = this_object();

  pending = all_inventory(ob);

  // recursively check the inventory of all
  // the items
  while( sizeof(pending) > 0 )
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

nomask object first_inventory(mixed ob)
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

  if (list && sizeof(list))
    return list[0];

  return nil;
}

// next_inventory - return the next object in the same inventory
// object next_inventory( object ob );
// Return the next object in the same inventory as 'ob'.

nomask object next_inventory(object ob)
{
  object * list, where;
  int i;

  where = environment(ob);

  if (!ob || !where)
    return nil;

  list = where->all_inventory();

  // impossible
  if ((i = member_array(ob, list)) == -1)
    return nil;

  // last object in inventory
  if (i >= sizeof(list) - 1)
    return nil;

  return list[i+1];
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

// id - function called by present() in order to identify an object
// int id( string an_id );

// The present() efunction calls id() to determine if a given object is named
// by a given string.  id() should return 1 if the object wishes to be known
// by the name in the string anId; it should return 0 otherwise.

// returns 0 by default, masked in /lib/core/basic/id.c for every object.c

int id(string str)
{
  return 0;
}

// present - find an object by id
// object present( mixed str);
// object present( mixed str, object ob );

// If an object for which id(str) returns true exists, return it.
// `str' can also be an object, in 'str' is searched for, instead of calling
// the function id().

// If `ob' is given, then the search is done in the inventory of `ob', otherwise
// the object is searched for in the inventory of the current object, and
// in the inventory of the environment of the current object.
nomask object present(mixed str, varargs object ob)
{
  object * where;
  string what;
  int num, i;

  // check if the object is in the inventory
  if (objectp(str))
  {
    if (ob && objectp(ob))
      where = all_inventory(ob);
    else
      where = all_inventory(this_object());

    if (member_array(str, where) != -1)
      return (object)str;

    return nil;
  }

  num = 1;

  // if we are looking for "sword 2"
  if (sscanf(str, "%s %d", what, num) >= 2)
    str = what;

  if (ob && objectp(ob))
    where = all_inventory(ob);
  else
    where = all_inventory(this_object()) +
            all_inventory(environment(this_object()));

  if (strlen(str))
  {
    for (i = 0; i < sizeof(where); i++)
    {
      if (where[i]->id(str) && (--num <= 0))
        return where[i];
    }
  }

  return nil;
}

