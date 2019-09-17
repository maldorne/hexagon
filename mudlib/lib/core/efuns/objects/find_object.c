
// find_object - find an object by file name
// object find_object( string str );
// Find the object with the file name 'str'.  If the object is a
// cloned object, then it can be found using the file name which
// would by returned if file_name() was called with it as the
// argument.

static nomask object find_object(string name)
{
  object obj;

  // if name has a trailing .c, let's get rid of it.
  if (strlen(name) > 2 && name[strlen(name)-2..strlen(name)-1] == ".c") 
    name = name[0..strlen(name)-3];

  obj = ::find_object(name);

  if (!obj)
    stderr(" - find_object: <" + name + "> (not found)\n");
  
  return obj;
}
