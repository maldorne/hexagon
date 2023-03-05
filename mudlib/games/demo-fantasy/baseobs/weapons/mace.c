inherit "/lib/weapon.c"; 

#include <translations/weapon.h>
#include <language.h>

void setup()
{
  set_base_weapon(LIGHT_MACE);
  add_alias(LIGHT_MACE);

  set_name(_LANG_BASEOBS_LIGHT_MACE);
  set_short(capitalize(_LANG_BASEOBS_LIGHT_MACE));
  add_alias(_LANG_BASEOBS_LIGHT_MACE_ALIASES);
 
  set_main_plural(capitalize(_LANG_BASEOBS_LIGHT_MACE_PLURALS[0]));
  add_plural(_LANG_BASEOBS_LIGHT_MACE_PLURALS);

  set_long(_LANG_BASEOBS_LIGHT_MACE_LONG);

  // in spanish
  set_gender(2);
} 
