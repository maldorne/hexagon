
// refactor from /lib/room.c, to be used both from rooms and location
// components, neverbot 01/2025

#include <translations/light.h>

static string dark_mess;

void create()
{
  dark_mess = _LANG_ROOM_TOO_DARK;
}

void set_dark_mess(string str) { dark_mess = str; }

private string end_line(string str)
{
  if (!strlen(str) || str[strlen(str) - 1] != '\n')
    return str + "\n";
  return str;
}

string query_dark_mess(int lvl)
{
  object what;
  // force update of the exit_string every time we are querying the 
  // description

  // if (!exit_string)
  //   exit_string = query_dirs_string();
  this_object()->query_dirs_string();

  // this is a location
  if (this_object()->query_location_component())
    what = this_object()->query_my_location();
  // this is a room
  else
    what = this_object();
  
  // every message is whole lines: it ends in a newline, so whatever the caller
  // adds after it (or the prompt) starts on a line of its own
  switch(lvl)
  {
    default:
      return end_line(_LANG_ROOM_LIGHT_DEF + " " + dark_mess);
    case 1: /* Total blackout */
      return end_line(dark_mess);
    case 2: /* pretty damn dark */
      return _LANG_ROOM_LIGHT_2 + this_object()->query_dirs_string() + ".\n";
    case 3: /* getting dim */
      return _LANG_ROOM_LIGHT_3 + what->short(0) + ".\n" + this_object()->query_dirs_string() + ".\n";
    case 4: /* slightly dazzled */
      return _LANG_ROOM_LIGHT_4 + what->short(0) + ".\n" + this_object()->query_dirs_string() + ".\n";
    case 5: /* very bright */
      return _LANG_ROOM_LIGHT_5 + this_object()->query_dirs_string() + ".\n";
    case 6:
      return _LANG_ROOM_LIGHT_6;
  }
}

mixed * stats()
{
  return ({ 
    ({ "Dark mess (nosave)", dark_mess, }) 
         });
}
