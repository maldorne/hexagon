
// Non-dgd efuns
// neverbot, 03/2014

// environment - return the environment of an object
// object environment( object ob );
// Return the containing object (environment) of 'ob'. If no argument
// is given, 'ob' defaults to this_object().  If the object is not
// inside anything, zero is returned.

object environment( varargs object ob )
{
  if (!ob)
    ob = this_object();

  return ob->_inner_environment();
}

// move_object - move current object to another environment
// void move_object( mixed dest );
// Move the current object into the object 'dest'.  dest should either be
// a filename or an object.

void move_object(mixed dest)
{
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


// Non-dgd efuns
// neverbot, 03/2014


// object array all_inventory( object ob );
// Returns an array of the objects contained inside of 'ob'.  If 'ob' is
// omitted, this_object() is used.

object * all_inventory(varargs object ob)
{
  if (!ob)
    ob = this_object();

  return ob->_inner_inventory();
}

// object *deep_inventory( object ob );
// Returns an array of the objects contained in the inventory
// of <ob> and all the objects contained in the inventories of
// those objects and so on. The return array of objects is flat,
// i.e. there is no structure reflecting internal containment
// relations.

object * deep_inventory(object ob)
{
  object * result;
  object * pending;
  int i;

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

