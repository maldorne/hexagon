
#include <living/races.h>
#include <translations/language.h>
#include <translations/races.h>
#include <language.h>

inherit STD_RACE;

void setup()
{
  set_limbs(2);
  // from 0 (very small) - 5 (human) - 10 (very big)
  set_body_size(6);
  set_name(_LANG_RACES_GNOLL_NAME);
  set_short(capitalize(_LANG_RACES_GNOLL_NAME));
  set_light_limits(LIGHT_STD_LOW, LIGHT_STD_HIGH);

  // If you have different cultures, the init_room will be set in each of them
  set_init_room("/games/demo-fantasy/areas/road/rooms/1.c");

  // do not allow new players with this race
  set_playable(0);
}

string query_desc(object ob)
{
  if (ob)
  {
    if ((int)ob->query_gender() == 1)
      return _LANG_RACES_GNOLL_DESC_MALE;
    return _LANG_RACES_GNOLL_DESC_FEMALE;
  }
  return "";
}

void start_player(object ob)
{ 
  ob->set_min(_LANG_RACES_MSG_IN_STD);
  ob->set_mout(_LANG_RACES_MSG_OUT_STD);
}

void set_racial_bonuses(object ob)
{
  ob->adjust_bonus_cha(2);
}

string * query_initial_languages()
{
  return ({ "goblin" });
}

int query_race_weight()
{
  return STD_WEIGHT;
}

string query_race_gender_string(object player, varargs int flag)
{
  if (!flag)
    return player->query_article() + " " + _LANG_RACES_GNOLL_GENDER_STRING;
  else
    return _LANG_RACES_GNOLL_GENDER_STRING;
}
