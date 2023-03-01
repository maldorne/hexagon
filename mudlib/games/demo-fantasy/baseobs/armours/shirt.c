inherit "/lib/armour.c"; 

#include <translations/armour.h>
#include <language.h>

void setup()
{ 
  set_base_armour(SHIRT);

  set_name(_LANG_BASEOBS_SHIRT);
  set_short(capitalize(_LANG_BASEOBS_SHIRT));
 
  set_main_plural(capitalize(pluralize(_LANG_BASEOBS_SHIRT)));
  add_plural(pluralize(_LANG_BASEOBS_SHIRT));

  set_long(_LANG_BASEOBS_SHIRT_LONG);

  // in spanish
  set_gender(2);
} 
