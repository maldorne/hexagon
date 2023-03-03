inherit "/lib/weapon.c"; 

#include <translations/weapon.h>
#include <language.h>

void setup()
{ 
  set_base_weapon(CLUB);
  add_alias(CLUB);

  set_name(_LANG_BASEOBS_CLUB);
  set_short(capitalize(_LANG_BASEOBS_CLUB));
 
  set_main_plural(capitalize(pluralize(_LANG_BASEOBS_CLUB)));
  add_plural(pluralize(_LANG_BASEOBS_CLUB));

  set_long(_LANG_BASEOBS_CLUB_LONG);

  // in spanish
  set_gender(1);
} 
