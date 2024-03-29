
// file_name - get the file name of an object
// string file_name( object ob );

// file_name() returns the name of the file from which 'ob' was loaded.
// If the object is a cloned object, then file_name() will not be an
// actual file on disk, but will be the name of the file from which the
// object was originally cloned, appended with an octothorpe (#) and the
// object instance number.  Object instance numbers start at 0 when the
// game is booted, and increase by one for each object cloned, hence the
// number is unique for each cloned object.  'ob' defaults to this_object()
// if not specified.

string file_name(varargs object ob)
{
  if (!ob) 
    ob = this_object();

  return ::object_name(ob);
}
