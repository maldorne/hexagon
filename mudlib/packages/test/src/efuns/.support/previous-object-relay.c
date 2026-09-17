// Helper for the previous_object test: calls back into the object that
// called it, so the callee sees a chain of two call_others.

mixed * relay(object back)
{
  return back->probe();
}
