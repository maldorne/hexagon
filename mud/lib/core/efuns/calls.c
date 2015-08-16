
#include <mud/mudos.h>

// call any local function, may be static (driver objects only)
// idea taken from mudos_alike_4_dgd mudlib

nomask mixed __call_other(string func, varargs mixed args...) 
{
  if (!mudlib_privileges()) 
  {
    stderr("Illegal __call_other on <" + object_name(this_object()) + ">, func: " + func + " \n");
    return nil;
  }

  return ::call_other(this_object(), func, args...);
}
