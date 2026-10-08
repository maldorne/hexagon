// #define TESTING 1
// Reviewed for CcMud, neverbot 6/03
// Removed the string about the moon, now there are three and it takes up a lot.

#include <areas/weather.h>
#include <translations/light.h>

inherit room        "/lib/room.c";
inherit extra_look  "/lib/core/basic/extra_look.c";
inherit night       "/lib/room/outside-night.c";

/* ok this is the out side room standard, It includes weather and
 * all that jazz
 */

void create()
{
  night::create();
  extra_look::create();
  room::create();

  add_property("location", "outside");
}

string long(string str, int dark)
{
  string s, ret;

  if (this_player())
    dark = (int)this_player()->check_dark(query_light());

  // looking to an item: its details cannot be made out in the dark either
  if (str && strlen(str))
  {
    if (dark)
      return query_dark_mess(dark);
    str = expand_alias(str);
    return items[str];
  }

  ret = "";

  // absolute darkness or glare: nothing but the message (see /lib/room/dark.c),
  // and a hint of the time of day when it contradicts what is seen
  if (dark && query_dark_hides_place(dark))
  {
    ret += query_dark_mess(dark);
    if ((dark == 1) && handler("weather", this_object())->query_day())
      ret += _LANG_ROOM_DARK_BUT_DAY;
    if ((dark == 6) && !handler("weather", this_object())->query_day())
      ret += _LANG_ROOM_BRIGHT_BUT_NIGHT;
    return ret;
  }

  // too dark or too bright for details: the description is lost, the rest of
  // the place is still seen
  if (dark)
    ret += query_dark_mess(dark);
  else if (this_object()->query_night_long() &&
           !handler("weather", this_object())->query_day())
    ret += this_object()->query_night_long();
  else
    ret += sprintf("\n   %-=*s\n", 
                   (this_user() ? this_user()->query_cols() : 79), 
                   "   " + query_long());

  s = calc_extra_look();

  if (s && strlen(s))
    ret += s;

  ret += (string)handler("weather", this_object())->weather_string(this_object());

  // refreshed every time: open and closed doors show in it
  query_dirs_string();

  if (exit_string)
    ret += exit_string + "\n";

  // return the long + the contents of the room.
  return ret + query_contents("");
}

// The day lights the place by a percentage of its own light; what is carried
// in (a torch) shines the same at any hour, as it does indoors.
int query_light()
{
  int i;
#ifdef TESTING
  i = 100;
#else
  i = (int)handler("weather", this_object())->query_darkness(this_object());
#endif

  return query_my_light() * i / 100 + query_int_light();
} /* query_light() */

int query_outside()
{
  return 1;
}

mixed * stats()
{
	return room::stats() + extra_look::stats();
}
