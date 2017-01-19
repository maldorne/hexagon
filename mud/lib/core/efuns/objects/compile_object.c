
static nomask object compile_object(string path, varargs string source...)
{
  mixed obj;
  string err;

  // if path has a trailing .c, let's get rid of it
  if (strlen(path) > 2 && path[strlen(path)-2..strlen(path)-1] == ".c") 
    path = path[0..strlen(path)-3];

  obj = ::find_object(path);

  // only non previously compiled code
  if (!obj)
  {
    stderr(" - compile_object: " + path + "\n");

    // errors during compilation will be reported to
    // the driver object to compile_error() 
    // even with this catch here

    if (source && (sizeof(source) > 0))
      err = catch(obj = ::compile_object(path, source...));
    else 
      err = catch(obj = ::compile_object(path)); 
  }
  else
  {
    // write(object_name(obj) + " was already compiled.\n");
    return obj;
  }

  if (err || !objectp(obj))
  {
    object initial;
    
    if (err)
    {
      stderr(" - compile_object: " + path + " (not able to compile)\n"+
             "                   " + err + "\n");
      initial = initial_object();

      if (initial && initial->query_coder())
        write(err + "\n");
    }
    else
      stderr(" - compile_object: " + path + " (not able to compile)\n");
  }

  return obj;
}
