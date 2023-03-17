inherit "/lib/weapon.c"; 

#include <translations/weapon.h>
#include <language.h>

void setup()
{ 
  set_base_weapon(STAFF);
  add_alias(STAFF);

  set_name(_LANG_BASEOBS_STAFF);
  set_short(capitalize(_LANG_BASEOBS_STAFF));
  add_alias(_LANG_BASEOBS_STAFF_ALIASES);

  set_main_plural(capitalize(_LANG_BASEOBS_STAFF_PLURALS[0]));
  add_plural(_LANG_BASEOBS_STAFF_PLURALS);

  set_long(_LANG_BASEOBS_STAFF_LONG);

  // in spanish
  set_gender(1);
} 
