
// load_object - find or load an object by file name
// object load_object( string str );
// Find the object with the file name 'str'.  If the file exists and the
// object hasn't been loaded yet, it is loaded.  Otherwise zero is returned.
// An error is thrown only if the object has compile errors.

static nomask object load_object(string name) 
{
  object obj;
  
  // if name has a trailing .c, let's get rid of it.
  if (strlen(name) > 2 && name[strlen(name)-2..strlen(name)-1] == ".c") 
    name = name[0..strlen(name)-3];

  stderr(" - load_object: " + name + " from: " + previous_program() + "\n");

  obj = find_object(name);
  
  if (!obj)
    obj = compile_object(name);
  
  return obj;
}
