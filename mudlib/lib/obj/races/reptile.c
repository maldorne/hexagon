
#include <living/races.h>
#include <language.h>
#include <translations/races.h>

inherit STD_RACE;

void setup()
{
  set_limbs(0);
  // from 0 (very small) - 5 (human) - 10 (very big)
  set_body_size(2);
  set_name(_LANG_RACES_REPTILE_NAME);
  set_short(capitalize(_LANG_RACES_REPTILE_NAME));
  set_light_limits(LIGHT_STD_LOW, LIGHT_STD_HIGH);

  // do not allow new players with this race
  set_playable(0);
}

string query_desc(object ob)
{
  return _LANG_RACES_REPTILE_DESC;
}

void start_player(object ob) 
{ 
  ob->set_min(_LANG_RACES_MSG_IN_SLITHER);
  ob->set_mout(_LANG_RACES_MSG_OUT_SLITHER);
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
  return STD_WEIGHT/2;
}

string query_race_gender_string(object player, varargs int flag)
{
  return "";
}

string * query_locations()
{
  return ({ _LANG_RACES_LOCATIONS_HEAD, _LANG_RACES_LOCATIONS_BODY, _LANG_RACES_LOCATIONS_TAIL, });
}

// returns enough info to hit in a body location
mixed obtain_location()
{
  float mult; // damage multiplier
  string name; // location name

  switch (random(15)) 
  {
    case 0:      mult = 2.0; name = _LANG_RACES_LOCATIONS_HEAD; break;
    case 1..10:  mult = 1.0; name = _LANG_RACES_LOCATIONS_BODY; break;
    case 11..15: mult = 0.5; name = _LANG_RACES_LOCATIONS_TAIL; break;
  }

  return ({ mult, name, _LANG_RACES_REPTILE_BODY });
}
