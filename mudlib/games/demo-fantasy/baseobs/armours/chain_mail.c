inherit "/lib/armour.c"; 

#include <translations/armour.h>
#include <language.h>

void setup()
{ 
  set_base_armour(CHAIN_MAIL);

  set_name(_LANG_BASEOBS_CHAIN_MAIL);
  set_short(capitalize(_LANG_BASEOBS_CHAIN_MAIL));
 
  set_main_plural(capitalize(pluralize(_LANG_BASEOBS_CHAIN_MAIL)));
  add_plural(pluralize(_LANG_BASEOBS_CHAIN_MAIL));

  set_long(_LANG_BASEOBS_CHAIN_MAIL_LONG);

  // in spanish
  set_gender(2);
} 
