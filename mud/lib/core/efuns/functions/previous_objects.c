
#include <trace.h>

static nomask object * previous_objects()
{
  int i;
  mixed ** trace, last;
  object * result;
  result = ({ });

  trace = call_trace();

  for (i = sizeof(trace) - 1; i >= 0; i--)
  {
    if (!last)
      last = trace[i][TRACE_OBJNAME];
    else if (trace[i][TRACE_OBJNAME] == last)
      continue;

    if (objectp(trace[i][TRACE_OBJNAME]))
      result += ({ trace[i][TRACE_OBJNAME] });
    else
      result += ({ find_object(trace[i][TRACE_OBJNAME]) });

    last = trace[i][TRACE_OBJNAME];
  }

  return result;
}
