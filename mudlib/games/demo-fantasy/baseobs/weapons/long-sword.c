inherit "/lib/weapon.c"; 

#include <translations/weapon.h>
#include <language.h>

void setup()
{ 
  set_base_weapon(LONG_SWORD);
  add_alias(LONG_SWORD);

  set_name(_LANG_BASEOBS_LONG_SWORD);
  set_short(capitalize(_LANG_BASEOBS_LONG_SWORD));
  add_alias(_LANG_BASEOBS_LONG_SWORD_ALIASES);

  set_main_plural(capitalize(_LANG_BASEOBS_LONG_SWORD_PLURALS[0]));
  add_plural(_LANG_BASEOBS_LONG_SWORD_PLURALS);

  set_long(_LANG_BASEOBS_LONG_SWORD_LONG);

  // in spanish
  set_gender(2);
} 
