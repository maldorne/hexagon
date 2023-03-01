inherit "/lib/weapon.c"; 

#include <translations/weapon.h>
#include <language.h>

void setup()
{ 
  set_base_weapon(LONG_SWORD);

  set_name(_LANG_BASEOBS_LONG_SWORD);
  set_short(capitalize(_LANG_BASEOBS_LONG_SWORD));
 
  set_main_plural(capitalize(pluralize(_LANG_BASEOBS_LONG_SWORD)));
  add_plural(pluralize(_LANG_BASEOBS_LONG_SWORD));

  set_long(_LANG_BASEOBS_LONG_SWORD_LONG);

  // in spanish
  set_gender(2);
} 
