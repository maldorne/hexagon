inherit "/lib/weapon.c"; 

#include <translations/weapon.h>
#include <language.h>

void setup()
{
  set_base_weapon(HAND_AXE);
  add_alias(HAND_AXE);

  set_name(_LANG_BASEOBS_HAND_AXE);
  set_short(capitalize(_LANG_BASEOBS_HAND_AXE));
  add_alias(_LANG_BASEOBS_HAND_AXE_ALIASES);
 
  set_main_plural(capitalize(_LANG_BASEOBS_HAND_AXE_PLURALS[0]));
  add_plural(_LANG_BASEOBS_HAND_AXE_PLURALS);

  set_long(_LANG_BASEOBS_HAND_AXE_LONG);

  // in spanish
  set_gender(2);
} 
