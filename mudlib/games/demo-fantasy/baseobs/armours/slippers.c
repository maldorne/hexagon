inherit "/lib/armour.c"; 

#include <translations/armour.h>
#include <language.h>

void setup()
{ 
  set_base_armour(SLIPPERS);

  set_name(_LANG_BASEOBS_SLIPPERS);
  set_short(capitalize(_LANG_BASEOBS_SLIPPERS));
 
  set_main_plural(capitalize(pluralize(_LANG_BASEOBS_SLIPPERS)));
  add_plural(pluralize(_LANG_BASEOBS_SLIPPERS));

  set_long(_LANG_BASEOBS_SLIPPERS_LONG);
} 
