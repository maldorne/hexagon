
// restore_object - restore values of variables from a file into an object
// int restore_object( string name, int flag );

// Restore values of variables for current object from file `name'. If the 
// optional second argument is 1, then all of the non-static variables are not 
// zeroed out prior to restore (normally, they are).

// In the case of an error, the affected variable will be left untouched
// and an error given.

static nomask int restore_object( string name, varargs int flag )
{
  debug("files", "restore_object: " + name + "\n");

  // DGD always set to zero all variables, so 
  // the mudos flag is ignored here
  return ::restore_object(name);
}
