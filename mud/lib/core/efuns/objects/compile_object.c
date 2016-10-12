
static nomask object compile_object(string path, varargs string source...)
{
  mixed obj;
  string err;

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
    // TODO different messages if not coder
    write(object_name(obj) + " was already compiled.\n");
    return nil;
  }

  if (err || !objectp(obj))
    stderr(" - compile_object: " + path + " (not able to compile)\n");

  return obj;
}
