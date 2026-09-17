
#include <trace.h>

// The objects in the call_other chain, nearest first: element 0 is the object
// that called the current function with call_other, element 1 the one that
// called that one, and so on. This follows the kfun previous_object(): local
// calls (including every efun, inherited from auto) stay within the current
// object, and each call_other steps back once. A destructed object leaves nil
// in its place.
static nomask object * previous_objects()
{
  mixed ** trace;
  object * result;
  mixed caller;
  int i;

  result = ({ });
  trace = call_trace();
  i = sizeof(trace) - 1;

  while (i > 0)
  {
    // back to the last call made with call_other
    while (i > 0 && !trace[i][TRACE_EXTERNAL])
      i--;

    if (i == 0)
      break;

    // the frame before it belongs to the caller
    i--;
    caller = trace[i][TRACE_OBJNAME];
    result += ({ objectp(caller) ? caller : find_object(caller) });
  }

  return result;
}
