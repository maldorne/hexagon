inherit "/lib/shield.c"; 

#include <translations/shield.h>
#include <language.h>

void setup()
{ 
  set_base_shield(SMALL_WOODEN_SHIELD);
  add_alias(SMALL_WOODEN_SHIELD);

  set_name(_LANG_BASEOBS_SMALL_WOODEN_SHIELD);
  set_short(capitalize(_LANG_BASEOBS_SMALL_WOODEN_SHIELD));
  add_alias(_LANG_BASEOBS_SMALL_WOODEN_SHIELD_ALIASES);
 
  set_main_plural(capitalize(_LANG_BASEOBS_SMALL_WOODEN_SHIELD_PLURALS[0]));
  add_plural(_LANG_BASEOBS_SMALL_WOODEN_SHIELD_PLURALS);

  set_long(_LANG_BASEOBS_SMALL_WOODEN_SHIELD_LONG);
} 
