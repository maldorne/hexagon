inherit "/lib/armour.c"; 

#include <translations/armour.h>
#include <language.h>

void setup()
{ 
  set_base_armour(HELMET);
  add_alias(HELMET);

  set_name(_LANG_BASEOBS_HELMET);
  set_short(capitalize(_LANG_BASEOBS_HELMET));
 
  set_main_plural(capitalize(pluralize(_LANG_BASEOBS_HELMET)));
  add_plural(pluralize(_LANG_BASEOBS_HELMET));

  set_long(_LANG_BASEOBS_HELMET_LONG);

  // in spanish
  set_gender(1);
} 
