inherit "/lib/armour.c"; 

#include <translations/armour.h>
#include <language.h>

void setup()
{ 
  set_base_armour(TROUSERS);
  add_alias(TROUSERS);

  set_name(_LANG_BASEOBS_TROUSERS);
  set_short(capitalize(_LANG_BASEOBS_TROUSERS));
 
  set_main_plural(capitalize(pluralize(_LANG_BASEOBS_TROUSERS)));
  add_plural(pluralize(_LANG_BASEOBS_TROUSERS));

  set_long(_LANG_BASEOBS_TROUSERS_LONG);

  // in spanish
  set_gender(2);
} 
