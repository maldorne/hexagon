// Bats: small flying mammals, furred, with membranous wings and fangs.
// The animal race is for the ones that stay on the ground.

#include <living/races.h>
#include <living/body-parts.h>
#include <living/skills.h>
#include <language.h>
#include <translations/races.h>

inherit STD_RACE;

void setup()
{
  set_limbs(0);
  // from 0 (very small) - 5 (human) - 10 (very big)
  set_body_size(1);
  set_name(_LANG_RACES_BAT_NAME);
  set_race_id(RACE_BAT);
  set_body_kind(BODY_BEAST);
  // membranous wings, cut off like any other wing
  add_body_part(PART_WING, part_spec(SKILL_DISMEMBERING, 10, 2, 4, 2, 1));
  set_short(capitalize(_LANG_RACES_BAT_NAME));
  set_light_limits(LIGHT_NIGHT_VISION_LOW, LIGHT_STD_HIGH);

  // fangs, 1d4
  add_unarmed_style("bat", _LANG_RACES_BAT_STYLE, 1, 4, 0,
    ({ ({ _LANG_RACES_BAT_STYLE_ATT_MSGS,
           _LANG_RACES_BAT_STYLE_DEF_MSGS,
           _LANG_RACES_BAT_STYLE_ROOM_MSGS, }), }));
  set_natural_style("bat");

  // do not allow new players with this race
  set_playable(0);
}

string query_desc(object ob)
{
  return _LANG_RACES_BAT_DESC;
}

void start_player(object ob)
{
  ob->set_min(_LANG_RACES_MSG_IN_FLY);
  ob->set_mout(_LANG_RACES_MSG_OUT_FLY);
}

void set_racial_bonuses(object ob)
{
  return;
}

string * query_initial_languages()
{
  return ({ });
}

int query_race_weight()
{
  return STD_WEIGHT/20;
}

string query_race_gender_string(object player, varargs int flag)
{
  return "";
}

string * query_locations()
{
  return ({ _LANG_RACES_LOCATIONS_HEAD, _LANG_RACES_LOCATIONS_BODY,
    _LANG_RACES_LOCATIONS_BAT_WING, _LANG_RACES_LOCATIONS_ANIMAL_LEG });
}

// returns enough info to hit in a body location
mixed obtain_location()
{
  float mult; // damage multiplier
  string name; // location name

  switch (random(20))
  {
    case 0..1:   mult = 2.0; name = _LANG_RACES_LOCATIONS_HEAD; break;
    case 2..11:  mult = 1.0; name = _LANG_RACES_LOCATIONS_BODY; break;
    case 12..17: mult = 0.5; name = _LANG_RACES_LOCATIONS_BAT_WING; break;
    case 18..20: mult = 0.5; name = _LANG_RACES_LOCATIONS_ANIMAL_LEG; break;
  }

  return ({ mult, name, _LANG_RACES_BAT_BODY });
}
