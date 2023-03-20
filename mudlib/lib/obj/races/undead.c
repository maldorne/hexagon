
// New races for Cc, neverbot 4/2003

#include <living/races.h>
#include <translations/language.h>
#include <translations/races.h>
#include <language.h>

inherit STD_RACE;

void setup()
{
  set_limbs(2);
  // from 0 (very small) - 5 (human) - 10 (very big)
  set_body_size(5);
  set_name(_LANG_RACES_UNDEAD_NAME);
  set_short(capitalize(_LANG_RACES_UNDEAD_NAME));
  set_light_limits(LIGHT_STD_LOW, LIGHT_STD_HIGH);

  // do not allow new players with this race
  set_playable(0);
}

string query_desc(object ob)
{
  return _LANG_RACES_UNDEAD_DESC;
}

void start_player(object ob)
{ 
  ob->set_min(_LANG_RACES_MSG_IN_STD);
  ob->set_mout(_LANG_RACES_MSG_OUT_STD);
}

void set_racial_bonuses(object ob)
{
  ob->adjust_bonus_con(-2);
  ob->adjust_bonus_wil(-2);
  ob->adjust_bonus_str(-1);
}

string * query_initial_languages()
{
  return ({ STD_LANG });
}

int query_race_weight()
{
  return STD_WEIGHT;
}

string query_race_gender_string(object player, varargs int flag)
{
  if (!flag)
    return player->query_article() + " " + _LANG_RACES_UNDEAD_GENDER_STRING;
  else
    return _LANG_RACES_UNDEAD_GENDER_STRING;
}
