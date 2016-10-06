
// destruct - remove an object
// void destruct( object ob );

// Completely destroy and remove object 'ob'. After the call to destruct().
// If 'ob' is this_object(), execution will continue, but it is best to return
// a value immediately.  All pointers to the object in any variable or structure
// will immediately become zero.  move_or_destruct() is called in all the
// objects inside of the object being destructed.

int destruct(varargs object ob) 
{
  stderr(" - destruct: <" + object_name(ob) + ">\n");

  if (!ob)
    ob = this_object();

  ::destruct_object(ob);
  
  return 1;
  // return (this_object() == nil);
}
