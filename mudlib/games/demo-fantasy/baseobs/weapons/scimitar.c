inherit "/lib/weapon.c"; 

#include <translations/weapon.h>
#include <language.h>

void setup()
{ 
  set_base_weapon(SCIMITAR);

  set_name(_LANG_BASEOBS_SCIMITAR);
  set_short(capitalize(_LANG_BASEOBS_SCIMITAR));
 
  set_main_plural(capitalize(pluralize(_LANG_BASEOBS_SCIMITAR)));
  add_plural(pluralize(_LANG_BASEOBS_SCIMITAR));

  set_long(_LANG_BASEOBS_SCIMITAR_LONG);

  // in spanish
  set_gender(2);
} 
