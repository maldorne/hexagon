// Created by Lummen 16-7-97

#include <language.h>

inherit "/lib/monster.c";

void setup()
{
  set_name(_LANG_NPCS_SNAKE_NAME);
  set_short(capitalize(_LANG_NPCS_SNAKE_NAME));

  set_main_plural(capitalize(pluralize(_LANG_NPCS_SNAKE_NAME)));
  add_plural(pluralize(_LANG_NPCS_SNAKE_NAME));

  set_random_stats(3, 16);
  set_race("reptile");
  set_level(3+random(3));
  set_gender(2); // for spanish language
  set_real_align(-30);

  set_long(_LANG_NPCS_SNAKE_LONG);
  set_join_fight_mess(_LANG_NPCS_SNAKE_JOIN_FIGHT);
}
