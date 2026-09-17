
// The object at the bottom of the call chain: the one the driver called to
// start the current execution.
static nomask object initial_object()
{
  mixed ** trace;
  mixed first;

  trace = call_trace();

  if (!sizeof(trace))
    return nil;

  first = trace[0][TRACE_OBJNAME];
  return objectp(first) ? first : find_object(first);
}
