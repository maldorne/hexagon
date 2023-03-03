inherit "/lib/weapon.c"; 

#include <translations/weapon.h>
#include <language.h>

void setup()
{ 
  set_base_weapon(DAGGER);
  add_alias(DAGGER);

  set_name(_LANG_BASEOBS_DAGGER);
  set_short(capitalize(_LANG_BASEOBS_DAGGER));
 
  set_main_plural(capitalize(pluralize(_LANG_BASEOBS_DAGGER)));
  add_plural(pluralize(_LANG_BASEOBS_DAGGER));

  set_long(_LANG_BASEOBS_DAGGER_LONG);

  // in spanish
  set_gender(2);
} 
