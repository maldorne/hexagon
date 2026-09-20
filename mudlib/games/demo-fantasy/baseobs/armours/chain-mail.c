inherit "/lib/armour.c"; 

#include <translations/armour.h>
#include <language.h>

void setup()
{ 
  set_base_armour(CHAIN_MAIL);
  add_alias(CHAIN_MAIL);

  set_name(_LANG_BASEOBS_CHAIN_MAIL);
  set_short(capitalize(_LANG_BASEOBS_CHAIN_MAIL));
  add_alias(_LANG_BASEOBS_CHAIN_MAIL_ALIASES);

  set_main_plural(capitalize(_LANG_BASEOBS_CHAIN_MAIL_PLURALS[0]));
  add_plural(_LANG_BASEOBS_CHAIN_MAIL_PLURALS);

  set_long(_LANG_BASEOBS_CHAIN_MAIL_LONG);

  // in spanish
  set_gender(2);
} 
