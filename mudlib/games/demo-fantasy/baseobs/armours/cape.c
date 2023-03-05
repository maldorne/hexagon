inherit "/lib/armour.c"; 

#include <translations/armour.h>
#include <language.h>

void setup()
{ 
  set_base_armour(CAPE);
  add_alias(CAPE);

  set_name(_LANG_BASEOBS_CAPE);
  set_short(capitalize(_LANG_BASEOBS_CAPE));
 
  set_main_plural(capitalize(pluralize(_LANG_BASEOBS_CAPE)));
  add_plural(pluralize(_LANG_BASEOBS_CAPE));

  set_long(_LANG_BASEOBS_CAPE_LONG);

  // in spanish
  set_gender(2);
} 
