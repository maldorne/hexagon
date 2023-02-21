inherit "/lib/shield.c"; 

#include <translations/shield.h>
#include <language.h>

void setup()
{ 
  set_base_shield(SMALL_WOODEN_SHIELD);

  set_name(_LANG_BASEOBS_SMALL_WOODEN_SHIELD);
  set_short(capitalize(_LANG_BASEOBS_SMALL_WOODEN_SHIELD));
 
  set_main_plural(capitalize(pluralize(_LANG_BASEOBS_SMALL_WOODEN_SHIELD)));
  add_plural(pluralize(_LANG_BASEOBS_SMALL_WOODEN_SHIELD));

  set_long(_LANG_BASEOBS_SMALL_WOODEN_SHIELD_LONG);
} 
