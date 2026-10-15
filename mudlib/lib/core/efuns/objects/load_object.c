
// load_object - find or load an object by file name
// object load_object( string str );
// Find the object with the file name 'str'.  If the file exists and the
// object hasn't been loaded yet, it is loaded.  Otherwise zero is returned.
// A loaded object is returned without checking the disk.
// An error is thrown only if the object has compile errors.

static nomask object load_object(string name)
{
  object obj;
  int * sizes;

  if (undefinedp(name) || !strlen(name))
    return nil;

  // if name has a trailing .c, let's get rid of it.
  if (strlen(name) > 2 && name[strlen(name)-2..strlen(name)-1] == ".c")
    name = name[0..strlen(name)-3];

  // a clone has no file of its own
  if (sscanf(name, "%*s#%*s") == 2)
    return nil;

  // the kfun, so a miss is not logged as a missing object
  obj = ::find_object(name);
  if (obj)
    return obj;

  // extra check: if the file does not exist, don't
  // even bother trying
  sizes = ::get_dir(name + ".c")[1];

  // no files
  if (!sizes || (sizeof(sizes) == 0) || (sizes[0] == -2) )
    return nil;

  stderr(" - load_object: " + name + " from: " + previous_program() + "\n");
  obj = compile_object(name);

  return obj;
}
