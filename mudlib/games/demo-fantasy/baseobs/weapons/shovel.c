inherit "/lib/weapon.c";

#include <translations/weapon.h>
#include <language.h>

void setup()
{
  set_base_weapon(SHOVEL);
  add_alias(SHOVEL);

  set_name(_LANG_BASEOBS_SHOVEL);
  set_short(capitalize(_LANG_BASEOBS_SHOVEL));

  set_main_plural(capitalize(pluralize(_LANG_BASEOBS_SHOVEL)));
  add_plural(pluralize(_LANG_BASEOBS_SHOVEL));

  set_long(_LANG_BASEOBS_SHOVEL_LONG);

  // in spanish
  set_gender(2);
}
