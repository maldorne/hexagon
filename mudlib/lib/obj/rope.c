// A rope: long and strong enough to take a person's weight or tie a load. It
// does nothing by itself; what it is good for belongs to whatever asks
// query_rope() of what somebody carries.

inherit "/lib/item.c";

#include <language.h>

#define ROPE_VALUE 20

void setup()
{
  set_name(_LANG_ROPE_NAME);
  set_short(capitalize(_LANG_ROPE_NAME));

  set_main_plural(capitalize(pluralize(_LANG_ROPE_NAME)));
  add_plural(pluralize(_LANG_ROPE_NAME));

  set_long(_LANG_ROPE_LONG);

  // "la cuerda" is feminine in spanish
  set_gender(2);

  set_value(ROPE_VALUE);
  set_weight(30);
}

int query_rope() { return 1; }
