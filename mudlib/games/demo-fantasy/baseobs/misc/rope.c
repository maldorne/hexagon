inherit "/lib/item.c";

#include <language.h>

void setup()
{
  set_name(_LANG_BASEOBS_ROPE);
  set_short(capitalize(_LANG_BASEOBS_ROPE));

  set_main_plural(capitalize(pluralize(_LANG_BASEOBS_ROPE)));
  add_plural(pluralize(_LANG_BASEOBS_ROPE));

  set_long(_LANG_BASEOBS_ROPE_LONG);

  // in spanish
  set_gender(2);

  set_value(1);
  set_weight(1);
}
