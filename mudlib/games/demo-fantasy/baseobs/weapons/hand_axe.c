inherit "/lib/weapon.c"; 

#include <translations/weapon.h>
#include <language.h>

void setup()
{ 
  set_base_weapon(HAND_AXE);

  set_name(_LANG_BASEOBS_HAND_AXE);
  set_short(capitalize(_LANG_BASEOBS_HAND_AXE));
 
  set_main_plural(capitalize(pluralize(_LANG_BASEOBS_HAND_AXE)));
  add_plural(pluralize(_LANG_BASEOBS_HAND_AXE));

  set_long(_LANG_BASEOBS_HAND_AXE_LONG);

  // in spanish
  set_gender(2);
} 
