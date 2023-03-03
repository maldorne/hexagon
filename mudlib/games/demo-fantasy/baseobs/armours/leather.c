inherit "/lib/armour.c"; 

#include <translations/armour.h>
#include <language.h>

void setup()
{ 
  set_base_armour(LEATHER);
  add_alias(LEATHER);

  set_name(_LANG_BASEOBS_LEATHER);
  set_short(capitalize(_LANG_BASEOBS_LEATHER));
  add_alias(_LANG_BASEOBS_LEATHER_ALIASES);
 
  set_main_plural(capitalize(_LANG_BASEOBS_LEATHER_PLURALS[0]));
  add_plural(_LANG_BASEOBS_LEATHER_PLURALS);

  set_long(_LANG_BASEOBS_LEATHER_LONG);

  // in spanish
  set_gender(2);
} 
