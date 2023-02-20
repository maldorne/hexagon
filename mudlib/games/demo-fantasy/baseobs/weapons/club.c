inherit "/lib/weapon.c"; 

#include <item/weapon.h>
#include <language.h>

void setup()
{ 
  set_base_weapon(CLUB);

  set_name(_LANG_BASEOBS_CLUB);
  set_short(capitalize(_LANG_BASEOBS_CLUB));
 
  set_main_plural(capitalize(pluralize(_LANG_BASEOBS_CLUB)));
  add_plural(pluralize(_LANG_BASEOBS_CLUB));

  set_long(_LANG_BASEOBS_CLUB_LONG);

  // in spanish
  set_gender(0);
} 
