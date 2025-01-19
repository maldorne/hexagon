// #define TESTING 1
// Reviewed for CcMud, neverbot 6/03
// Removed the string about the moon, now there are three and it takes up a lot.

#include <areas/weather.h>

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

  // looking to an item
  if (str && strlen(str))
  {
    str = expand_alias(str);
    return items[str];
  }

  ret = "";

  if (this_player())
    dark = (int)this_player()->check_dark(query_light());

  switch (dark) /* O.K. how much can we see at night */
  {
    default: /* can see anyway */
      /* night... */
      if (this_object()->query_night_long() && !handler(WEATHER_HANDLER)->query_day())
        ret += this_object()->query_night_long();
      else
        ret += sprintf("\n   %-=*s\n", 
                       (this_user() ? this_user()->query_cols() : 79), 
                       "   " + query_long());
      break;

    case 1..3: /* too dark */
      ret += ::query_dark_mess(dark);
      if (handler(WEATHER_HANDLER)->query_day())
        ret += "\nA pesar de esta oscuridad parece ser de día.\n";
      return ret;

    case 4..6: /* too bright */
      ret += ::query_dark_mess(dark);
      if (!handler(WEATHER_HANDLER)->query_day())
        ret += ".\nA pesar de esta luz parece ser de noche.\n";
      return ret;
  }

  /* if we got here we can see */

  s = calc_extra_look();

  if (s && strlen(s))
    ret += s;

  ret += (string)handler(WEATHER_HANDLER)->weather_string(this_object());

  // this will update exit_string if needed
  if (!exit_string)
    query_dirs_string();

  if (exit_string)
    ret += exit_string + "\n";

  // return the long + the contents of the room.
  return ret + query_contents("");
}

/* percentage system */
int query_light()
{
  int i;
#ifdef TESTING
  i = 100;
#else
  i = (int)handler(WEATHER_HANDLER)->query_darkness(this_object());
#endif

  return ::query_light()*i/100;
} /* query_light() */

int query_outside()
{
  return 1;
}

mixed * stats()
{
	return room::stats() + extra_look::stats();
}
