// location-cmd.c -- base of the admin commands that work on the location the
// admin stands in, and on its area: building the map, staffing and housing a
// settlement. It lives outside the command directories, so the dispatcher never
// takes it for a verb.

#include <mud/cmd.h>

inherit CMD_BASE;

/**
 * The location `me` stands in, or nil when it is a plain room or nowhere.
 */
static object location_of(object me)
{
  object env;

  env = me ? environment(me) : nil;
  return (env && env->query_location()) ? env : nil;
}

/**
 * The area of the location `me` stands in, or nil.
 */
static object area_of(object me)
{
  object loc;

  loc = location_of(me);
  return loc ? loc->query_area() : nil;
}

/**
 * The words of an argument, empty ones dropped; none for nil.
 */
static string * words(string str)
{
  return (str && strlen(str)) ? explode(str, " ") - ({ "" }) : ({ });
}

/**
 * The words from `from` on, joined again with single spaces: the free text at
 * the end of a command (a title, a description, a message).
 */
static string rest_of(string * args, int from)
{
  return from < sizeof(args) ? implode(args[from ..], " ") : "";
}

/**
 * Lays rows out in columns, the first row being the header, underlined.
 */
static string columns(string * * rows)
{
  string out;
  int * width;
  int i, j, k;

  if (!sizeof(rows))
    return "";

  width = allocate_int(sizeof(rows[0]));
  for (i = 0; i < sizeof(rows); i++)
    for (j = 0; j < sizeof(rows[i]); j++)
    {
      int l;
      l = strlen(rows[i][j], TRUE);
      if (l > width[j])
        width[j] = l;
    }

  out = "";
  for (i = 0; i < sizeof(rows); i++)
  {
    out += " ";
    for (j = 0; j < sizeof(rows[i]); j++)
      out += sprintf(" %-*s", width[j], rows[i][j]);
    out += "\n";

    if (i == 0)
    {
      out += " ";
      for (j = 0; j < sizeof(rows[i]); j++)
      {
        out += " ";
        for (k = 0; k < width[j]; k++)
          out += "-";
      }
      out += "\n";
    }
  }

  return out;
}
