

// Non-dgd efuns
// neverbot, 03/2014

object * previous_objects(varargs int step)
{
  object result;
  int arg;

  if (!step)
    arg = 0;
  else
    arg = step;

  result = previous_object(arg);

  if (result == nil)
    return ({ });

  return ({ result }) + previous_objects(arg + 1);

}

object previous_object(varargs int number)
{
  if (!number || (number != -1))
    return ::previous_object(number);
  else
    return previous_objects(1)[0];
}

/* The previous_function() function. It reports the name of the
   function in previous_object() that called the function which
   calls previous_function(). (Read that a couple times until it
   makes sense.)
   It's just a front end that extracts the particular information
   from the call_trace() kfun.
*/
static nomask string previous_function() 
{
  mixed *trace;
  mixed *elem;
  int size;

  trace = call_trace();
  size = sizeof(trace);

  // The last element is this function. The second to last element is
  //   the function that requested previous_function(). The third to
  //   last element is the function before that: that's the one we want.    
  if (size < 3) 
    return nil;
  
  elem = trace[size-3];
  
  // Element 2 is the function name. 
  return elem[2];
}

/*
 * Give a path to a simul_efun file. Observe that it is a string returned,
 * not an object. But the object has to be loaded here. Return 0 if this
 * feature isn't wanted.
 */
// string get_simul_efun() 
// {
//   string fname;
//   fname = "/secure/simul_efun";
  
//   if (catch(call_other(fname, "??"))) 
//   {
//     write("Failed to load " + fname + "\n");
//     shutdown();
//     return 0;
//   }
//   return fname;
// }

