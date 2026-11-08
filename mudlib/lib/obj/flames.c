// The flames of a burning location: they only show that there is a fire. The
// burning is done by the fire component (/lib/location/components/fire.c),
// which puts this fixture in the location and takes it away when the fire ends.

#include <language.h>

inherit "/lib/obj/fixture.c";

void setup()
{
  set_name(_LANG_FLAMES_NAME);
  add_alias(_LANG_FLAMES_ALIASES);
  set_short(_LANG_FLAMES_SHORT);
  set_main_plural(_LANG_FLAMES_SHORT);
  add_plural(_LANG_FLAMES_PLURALS);
  set_long(_LANG_FLAMES_LONG);

  // "las llamas" is feminine in spanish
  set_gender(2);
}
