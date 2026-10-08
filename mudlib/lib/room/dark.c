// refactor from /lib/room.c, to be used both from rooms and location
// components, neverbot 01/2025
//
// Darkness levels, as the viewer's race reports them (check_dark):
//   0      normal sight: everything is seen
//   2..5   too dark or too bright to make out details: the place's
//          description is replaced by one of these messages, while its name,
//          weather, exits and contents are still seen
//   1, 6   absolute darkness or blinding light: only the message is seen

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

// Whether at this level nothing of the place can be seen but the message.
int query_dark_hides_place(int lvl)
{
  return (lvl == 1) || (lvl == 6) || (lvl < 0) || (lvl > 6);
}

// The line that stands for what cannot be seen at this level. It always ends
// in a newline, so whatever the caller adds after it starts on its own line.
string query_dark_mess(int lvl)
{
  switch (lvl)
  {
    case 1: /* total blackout */
      return end_line(dark_mess);
    case 2: /* pretty damn dark */
      return _LANG_ROOM_LIGHT_2;
    case 3: /* getting dim */
      return _LANG_ROOM_LIGHT_3;
    case 4: /* slightly dazzled */
      return _LANG_ROOM_LIGHT_4;
    case 5: /* very bright */
      return _LANG_ROOM_LIGHT_5;
    case 6: /* blinded */
      return _LANG_ROOM_LIGHT_6;
    default:
      return end_line(_LANG_ROOM_LIGHT_DEF + " " + dark_mess);
  }
}

mixed * stats()
{
  return ({
    ({ "Dark mess (nosave)", dark_mess, })
         });
}
