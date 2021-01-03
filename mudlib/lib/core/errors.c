
// * void runtime_error(string error, int caught, mixed **trace)
//     A runtime error has occurred.
//
// * void atomic_error(string error, int atom, mixed **trace)
//     A runtime error has occurred in atomic code.
//
// * void compile_error(string file, int line, string error)
//     A compile-time error has occurred.

#include <kernel.h>
#include <mud/config.h>

nomask string runtime_error(string error, int caught, int ticks)
{
  if (!LOG_CAUGHT_ERRORS && caught)
    return nil;

  if (LOG_FULL_TRACE)
    return show_trace();
  else
    return last_call();
}

string atomic_error(string error, int atom, mixed **trace)
{
  return "lib > core > handlers > errors :: atomic_error\n";
  // load_object(DRIVER)->driver_message("An atomic error happened!");
}

string compile_error(string file, int line, string error)
{
  return file + ", " + line + ": " + error + "\n";
}




