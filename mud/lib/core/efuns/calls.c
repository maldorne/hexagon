
#include <mud/mudos.h>

static nomask mixed call_other(mixed obj, string func, mixed args...)
{
  object * shadows;

  if (!obj || !objectp(obj))
    return ::call_other(obj, func, args...);

  // ::call_other(DRIVER, "log_driver", " - masked call_other: <" +
  //   object_name(obj) + "> " + func + "\n");

  shadows = ::call_other(obj, "_query_shadows");

  if (arrayp(shadows) && sizeof(shadows))
  {
    int i, j;

    // look for the shadow corresponding the previous_program
    //  (not the same as previous_object()!!)
    for (j = 0; j < sizeof(shadows); j++)
    {
      // previous shadowed call found
      if (base_name(shadows[j]) == previous_program())
        break;
    }

    // from last shadow (more "external") to the older one
    // (more "internal", near the shadowed object)
    for (i = j - 1; i >= 0; i--)
    {
      string file;
      file = base_name(shadows[i]);

      // conditions
      // 1) the shadow is not calling itself (infinite loop)
      // 2) the shadow has the function implemented (not inherited)
      // 3) the shadow is not calling another previous shadow
      //    (because of the outer for loop)

      if (file == function_object(func, shadows[i]))
      {
        // ::call_other(DRIVER, "log_driver", "Redirect "+func+" to : <"+object_name(shadows[i])+">\n");
        return ::call_other(shadows[i], func, args...);
      }
    }
  }

  return ::call_other(obj, func, args...);
}

// call any local function, may be static (driver objects only)
// idea taken from mudos_alike_4_dgd mudlib

nomask mixed __call_other(string func, varargs mixed args...)
{
  if (!mudlib_privileges())
  {
    stderr(" *** illegal __call_other on <" + object_name(this_object()) + ">, func: " + func + " \n");
    return nil;
  }

  return call_other(this_object(), func, args...);
}

// call_out - delayed function call in same object
// void call_out( function fun, int delay, mixed arg, ... );
// Set up a call to 'fun'.  If fun is a string, it is interpreted as the
// name of a function in this_object().  The call will take place 'delay'
// seconds later, with the arguments 'arg' and following provided.

static nomask int call_out(string func, int delay, varargs mixed args...)
{
  int ret;

  // the real call_out
  ret = ::call_out("__call_out", delay, this_object(), this_player(), this_user(), func, args...);

  if (ret)
    MUDOS->_store_call_out(this_object(), ret, func, delay, args...);

  return ret;
}

nomask int __call_out(object ob, object player, object user, string func, varargs mixed args...)
{
  return MUDOS->_call_out(ob, player, user, func, args...);
}

// remove_call_out - remove a pending call_out
// int remove_call_out( int handle );

static nomask int remove_call_out(int handle)
{
  int ret;

  // the real remove_call_out
  ret = ::remove_call_out(handle);

  if (ret)
    MUDOS->_remove_call_out(this_object(), handle);

  return ret;
}

// find_call_out - find a call out scheduled to be called next
// int find_call_out( string func );
// Find the first call out due to be executed for function 'func', and
// return the time left. If it is not found, then return -1.

static nomask int find_call_out(string func)
{
  return MUDOS->_find_call_out(this_object(), func);
}

// call_out_info - get pending call_out() information
// array call_out_info();
// Get information about all pending call outs. An array is returned,
// where every item in the array consists 4 elements: the object,
// the function, the delay to go, and the optional argument.

static nomask mixed * call_out_info()
{
  return MUDOS->_call_out_info();
}
