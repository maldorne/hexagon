inherit "/lib/armour.c"; 

#include <translations/armour.h>
#include <language.h>

void setup()
{ 
  set_base_armour(CLOAK);
  add_alias(CLOAK);

  set_name(_LANG_BASEOBS_CLOAK);
  set_short(capitalize(_LANG_BASEOBS_CLOAK));
 
  set_main_plural(capitalize(pluralize(_LANG_BASEOBS_CLOAK)));
  add_plural(pluralize(_LANG_BASEOBS_CLOAK));

  set_long(_LANG_BASEOBS_CLOAK_LONG);

  // in spanish
  set_gender(1);
} 
