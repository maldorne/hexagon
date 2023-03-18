
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
  string head;

  head = "--- error" + (caught ? " (caught)" : "") + " ---\n";

  if (LOG_FULL_TRACE)
    return head + error + ", trace:\n" + show_trace() + "---\n";
  else
    return head + error + ", in:\n" + last_call() + "---\n";
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




