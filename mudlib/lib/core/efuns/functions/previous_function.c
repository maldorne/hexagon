
// previous_function - return the name of the calling function
// string previous_function();
//
// Returns the name of the function that called the function in which
// previous_function() is invoked, or nil if there is no such caller.
// Implemented on top of call_trace() rather than relying on driver state.

static nomask string previous_function()
{
  mixed * trace;

  trace = call_trace();

  // last element = this function
  // second to last element = the function that requested previous_function()
  // third to last element is the function before that: that's the one we want
  if (sizeof(trace) < 3)
    return nil;

  // index 2 of a frame is the function name
  return trace[sizeof(trace) - 3][2];
}
