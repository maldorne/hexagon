// Scorpions: arachnids with a pair of pincers and a tail ending in a stinger.

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
  set_name(_LANG_RACES_SCORPION_NAME);
  set_race_id(RACE_SCORPION);
  set_body_kind(BODY_INSECT);
  // the pincers and the stinger tail
  add_body_part(PART_CLAWS, part_spec(SKILL_PULLING, 10, 2, 6, 2, 0));
  add_body_part(PART_TAIL, part_spec(SKILL_DISMEMBERING, 10, 1, 8, 1, 1));
  set_short(capitalize(_LANG_RACES_SCORPION_NAME));
  set_light_limits(LIGHT_STD_LOW, LIGHT_STD_HIGH);

  // pincers and sting, 1d4+1
  add_unarmed_style("scorpion", _LANG_RACES_SCORPION_STYLE, 1, 4, 1,
    ({ ({ _LANG_RACES_SCORPION_STYLE_ATT_MSGS,
           _LANG_RACES_SCORPION_STYLE_DEF_MSGS,
           _LANG_RACES_SCORPION_STYLE_ROOM_MSGS, }), }));
  set_natural_style("scorpion");

  // do not allow new players with this race
  set_playable(0);
}

string query_desc(object ob)
{
  return _LANG_RACES_SCORPION_DESC;
}

void start_player(object ob)
{
  ob->set_min(_LANG_RACES_MSG_IN_SCUTTLE);
  ob->set_mout(_LANG_RACES_MSG_OUT_SCUTTLE);
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
  return STD_WEIGHT/50;
}

string query_race_gender_string(object player, varargs int flag)
{
  return "";
}

string * query_locations()
{
  return ({ _LANG_RACES_LOCATIONS_HEAD, _LANG_RACES_LOCATIONS_BODY,
    _LANG_RACES_LOCATIONS_PINCER, _LANG_RACES_LOCATIONS_STINGER,
    _LANG_RACES_LOCATIONS_INSECT_LEG });
}

// returns enough info to hit in a body location
mixed obtain_location()
{
  float mult; // damage multiplier
  string name; // location name

  switch (random(20))
  {
    case 0:      mult = 2.0; name = _LANG_RACES_LOCATIONS_HEAD; break;
    case 1..8:   mult = 1.0; name = _LANG_RACES_LOCATIONS_BODY; break;
    case 9..12:  mult = 0.5; name = _LANG_RACES_LOCATIONS_PINCER; break;
    case 13..15: mult = 0.5; name = _LANG_RACES_LOCATIONS_STINGER; break;
    case 16..20: mult = 0.5; name = _LANG_RACES_LOCATIONS_INSECT_LEG; break;
  }

  return ({ mult, name, _LANG_RACES_SCORPION_BODY });
}
