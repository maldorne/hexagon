
// desc — coder cmd to set the location's _specific_long: the author
// prose that no component can infer. See location.c::long for how it
// composes with component hook_long contributions.
//
//   desc                 print the current _specific_long and the
//                        _original_long for reference
//   desc <text>          set _specific_long inline (the whole rest of
//                        the line goes in verbatim)
//   desc --reset         clear _specific_long; the long body falls
//                        through to components / _original_long

#include <mud/cmd.h>
#include <room/location.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(({ "desc" }));
  set_usage("desc\n" +
            "desc <text>\n" +
            "desc --reset");
  set_help("Set or inspect the location's specific long description.\n" +
           "\n" +
           "  desc                  Show the current _specific_long and\n" +
           "                        the _original_long (captured at\n" +
           "                        room2loc time).\n" +
           "  desc <text>           Set _specific_long to the rest of\n" +
           "                        the line. Single line only; for\n" +
           "                        multi-paragraph longs, edit the\n" +
           "                        location's .o file by hand.\n" +
           "  desc --reset          Clear _specific_long; the body of\n" +
           "                        long() then falls through to the\n" +
           "                        component chain and finally to\n" +
           "                        _original_long.\n");
}

static int cmd(string str, object me, string verb)
{
  object here;
  string current, original;

  here = environment(me);
  if (!here)
  {
    notify_fail("You are nowhere.\n");
    return 0;
  }
  if (!here->query_location())
  {
    notify_fail("You are not in a location.\n");
    return 0;
  }

  if (!str || !strlen(str))
  {
    current  = here->query_specific_long();
    original = here->query_original_long();
    write("_specific_long:\n  " +
          (current && strlen(current) ? "\"" + current + "\"" : "(unset)") +
          "\n");
    write("_original_long:\n  " +
          (original && strlen(original) ? "\"" + original + "\"" : "(unset)") +
          "\n");
    return 1;
  }

  if (str == "--reset")
  {
    here->set_specific_long("");
    here->save_me();
    write("Cleared _specific_long.\n");
    return 1;
  }

  here->set_specific_long(str);
  here->save_me();
  write("Set _specific_long.\n");
  return 1;
}
