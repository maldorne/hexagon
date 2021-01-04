
static nomask mixed call_other(mixed obj, string func, mixed args...)
{
  object * shadows;

  if (!stringp(obj) && !objectp(obj))
  {
    if (this_object())
      error("illegal call_other on <" + object_name(this_object()) + ">, target object not provided");
    else
      error("illegal call_other, target object not provided, and this_object() does not exist");

    return nil;
  }

  if (stringp(obj))
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
