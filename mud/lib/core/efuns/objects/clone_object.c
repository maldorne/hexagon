
// clone_object - load a copy of an object
// object clone_object( string name, ... );
// object new( string name, ... );

// Create a new object from the file 'name', and give it a new unique
// name (by adding #xxx on to the end of the name).  Returns the new object.
// The object shares the program of the object 'name', but has its own
// set of variables.  The second and following arguments are passed to
// create()

static nomask object clone_object(mixed what) 
{
  if (stringp(what))
  {
    object blueprint;
    
    stderr(" - clone_object from \"" + what + "\"\n");
    blueprint = load_object(what);

    if (blueprint)
      return clone_object(blueprint);
    return nil;
  }

  if (objectp(what))
    stderr(" - clone_object from <" + object_name(what) + ">\n");

  return ::clone_object(what);
}
