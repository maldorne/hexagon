inherit "/lib/armour.c"; 

#include <translations/armour.h>
#include <language.h>

void setup()
{ 
  set_base_armour(LEATHER);

  set_name(_LANG_BASEOBS_LEATHER);
  set_short(capitalize(_LANG_BASEOBS_LEATHER));
 
  set_main_plural(capitalize(pluralize(_LANG_BASEOBS_LEATHER)));
  add_plural(pluralize(_LANG_BASEOBS_LEATHER));

  set_long(_LANG_BASEOBS_LEATHER_LONG);

  // in spanish
  set_gender(2);
} 
