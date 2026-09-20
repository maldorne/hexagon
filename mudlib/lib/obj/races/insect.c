#include <living/races.h>
#include <language.h>
#include <translations/races.h>

inherit STD_RACE;

void setup()
{
  set_limbs(0);
  // from 0 (very small) - 5 (human) - 10 (very big)
  set_body_size(1);
  set_name(_LANG_RACES_INSECT_NAME);
  set_short(capitalize(_LANG_RACES_INSECT_NAME));
  set_light_limits(LIGHT_STD_LOW, LIGHT_STD_HIGH);

  // do not allow new players with this race
  set_playable(0);
}

string query_desc(object ob)
{
  return _LANG_RACES_INSECT_DESC;
}

void start_player(object ob)
{
  // No move-message override: an unset slot already resolves to the
  // current-language default (see movement.c).
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
  return STD_WEIGHT/10;
}

string query_race_gender_string(object player, varargs int flag)
{
  return "";
}

string * query_locations()
{
  return ({ _LANG_RACES_LOCATIONS_HEAD, _LANG_RACES_LOCATIONS_BODY,
    _LANG_RACES_LOCATIONS_INSECT_LEG, _LANG_RACES_LOCATIONS_INSECT_WING });
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
    case 12..16: mult = 0.5; name = _LANG_RACES_LOCATIONS_INSECT_LEG; break;
    case 17..20: mult = 0.5; name = _LANG_RACES_LOCATIONS_INSECT_WING; break;
  }

  return ({ mult, name, _LANG_RACES_INSECT_BODY });
}
