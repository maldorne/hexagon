
#include "functions/previous_objects.c"
#include "functions/initial_object.c"


static nomask object * all_previous_objects()
{
  return previous_objects();
}

// previous_object - returns the object(s) that called the current function
// object previous_object();
// mixed previous_object(int x);

// Returns an object pointer to the object, if any, that called current function.
// Note that local function calls do not set previous_object() to the current
// object, but leave it unchanged.  If passed a positive integer, it goes back
// the given number of previous objects in the calling chain.
// previous_object(0) is the same as previous_object(), previous_object(1) is
// the previous object's previous_object(), etc.  previous_object(-1) returns
// an array containing all of the previous objects.

static nomask mixed previous_object(varargs int number)
{
  // if (undefinedp(number))
  //   return ::previous_object();
  // else if (number != -1)
  //   return ::previous_object(number);
  // else
  // return previous_objects();

  // fix, neverbot 05/19,
  // ::previous_object does not store every call_other
  // but the call_trace() do, so we'll use our
  // previous_objects() efun, which uses call_trace

  object * prevs;
  prevs = previous_objects();

  if (undefinedp(number))
    return prevs[0];

  if (number == -1)
    return prevs;

  if ((number >= 0) && (number < sizeof(prevs)))
    return prevs[number];

  return nil;
}

static nomask string previous_function()
{
  mixed * trace;

  trace = call_trace();

  // last element = this function
  // second to last element = the function that requested previous_function()
  // third to last element is the function before that: that's the one we want
  if (sizeof(trace) < 3)
    return nil;

  // second element is the function name
  return trace[sizeof(trace) - 3][2];
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


// evaluate - evaluate a function pointer
// mixed evaluate(mixed f, ...);
// If f is a function, f is called with the rest of the arguments.
// Otherwise, f is returned.  evaluate(f, ...) is the same as (*f)(...).

static mixed evaluate(mixed f, varargs mixed args...)
{
  if (stringp(f))
  {
    if (function_object(f, this_object()))
      return call_other(this_object(), f, args);
  }

  return f;
}

// function_exists - find the file containing a given function in an object
// string function_exists( string str, object ob );

// Return the file name of the object that defines the function 'str' in
// object 'ob'. The returned value can be other than 'file_name(ob)' if the
// function is defined by an inherited object.

// 0 is returned if the function was not defined.

// Note that function_exists() does not check shadows, or functions that cannot
// be called from outside the object.

static nomask string function_exists(string str, object ob)
{
  return function_object(str, ob);
}
