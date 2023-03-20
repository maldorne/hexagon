
#include <common/properties.h>
#include <translations/combat.h>
#include <living/death.h>
#include <language.h>

inherit UNDEAD_OBS_PATH + "base-undead";

void setup()
{
  set_name(_LANG_SKELETON_NAME);
  set_short(capitalize(_LANG_SKELETON_NAME));
  
  set_main_plural(capitalize(pluralize(_LANG_SKELETON_NAME)));
  add_plural(pluralize(_LANG_SKELETON_NAME));

  set_long(_LANG_SKELETON_DESC);
  
  set_class_ob("/lib/obj/classes/fighter.c");
  set_race_ob("/lib/obj/races/undead.c");
  
  set_level(5);
  set_str(25);
  set_int(2);
  set_wis(2);
  set_dex(5);
  set_con(25);
  set_cha(2);

  set_real_align(-1000);
  set_ext_align(-1000);

  set_kill_xp(0);

  add_property(MAGIC_STR, 100);
  add_property(FIRE_STR, -10);
  add_property(COLD_STR, 110);
  add_property(GOOD_STR, -30);
  add_property(EVIL_STR, 100);
  add_property(DEATH_STR, 100);
  
  set_aggressive(1);
  add_property(UNDEAD, 1);
}
