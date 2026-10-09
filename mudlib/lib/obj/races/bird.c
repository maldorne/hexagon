// Birds: anything with feathers and wings, from a sparrow to a hen. Kept apart
// from the animal race, which is for mammals, the ones with a hide to skin.

#include <living/races.h>
#include <language.h>
#include <translations/races.h>

inherit STD_RACE;

void setup()
{
  set_limbs(0);
  // from 0 (very small) - 5 (human) - 10 (very big)
  set_body_size(1);
  set_name(_LANG_RACES_BIRD_NAME);
  set_race_id(RACE_BIRD);
  set_short(capitalize(_LANG_RACES_BIRD_NAME));  
  set_light_limits(LIGHT_STD_LOW, LIGHT_STD_HIGH);

  // do not allow new players with this race
  set_playable(0);
}

string query_desc(object ob)
{
  return _LANG_RACES_BIRD_DESC;
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
  return STD_WEIGHT/20;
}

string query_race_gender_string(object player, varargs int flag)
{
  return "";
}

string * query_locations()
{
  return ({ _LANG_RACES_LOCATIONS_HEAD, _LANG_RACES_LOCATIONS_BEAK,
    _LANG_RACES_LOCATIONS_BODY, _LANG_RACES_LOCATIONS_BIRD_WING,
    _LANG_RACES_LOCATIONS_ANIMAL_LEG });
}

// returns enough info to hit in a body location
mixed obtain_location()
{
  float mult; // damage multiplier
  string name; // location name

  switch (random(20))
  {
    case 0:      mult = 2.0; name = _LANG_RACES_LOCATIONS_HEAD; break;
    case 1:      mult = 0.5; name = _LANG_RACES_LOCATIONS_BEAK; break;
    case 2..11:  mult = 1.0; name = _LANG_RACES_LOCATIONS_BODY; break;
    case 12..16: mult = 0.5; name = _LANG_RACES_LOCATIONS_BIRD_WING; break;
    case 17..20: mult = 0.5; name = _LANG_RACES_LOCATIONS_ANIMAL_LEG; break;
  }

  return ({ mult, name, _LANG_RACES_BIRD_BODY });
}
