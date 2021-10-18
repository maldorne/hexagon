
#include <mud/secure.h>

// save_object - save the values of variables in an object into a file

// int save_object( string name, int flag );

// Save all values of non-static variables in this object in the file 'name'.
// valid_write() in the master object determines whether this is allowed.
// If the optional second argument is 1, then variables that are zero (0) are also
// saved (normally, they aren't).  Object variables and function pointers
// always save as 0.  save_object() returns 1 for success, 0 for failure.

static nomask int save_object( string name, varargs int flag )
{
  int len;

  debug("files", "save_object: " + name + "\n");

  // always use .o extension for saved files
  len = strlen(name);
  if (len < 2 || name[len - 2 ..] != ".o")
    name += ".o";

  if (!SECURE->valid_write(name, this_object(), "save_object"))
  {
    debug("files", "save_object invalid write: " + name + " <"+object_name(this_object())+"> "+
      "<save_object>\n");
    return 0;
  }

  // DGD always set to zero all variables, so 
  // the mudos flag is ignored here
  ::save_object(name);

  return 1;
}
