
// New system to see enemies, neverbot 6/03

// Players like different ways of seeig other players/npcs 
// so we can customize this
// Options:
//    alignment  : red color if objects are enemies
//    difficulty : different colors according to the levels of both objects

// Pending:
//    race or culture : different colors to different cultures, nations?

#include <basic/alignment.h>
#include <translations/pov.h>

string query_color(object me, object he)
{
  int al_me, al_him;
  int pov;
  string pov_name;

  pov = me->user()->query_pov();
  pov_name = POV_TYPES[pov];

  if (!me || !he || (me == he))
    return "";

  al_me = (int)me->query_real_align();
  al_him = (int)he->query_ext_align();

  // color according to the type of pov
  if (pov_name == POV_ALIGNMENT && ALIGN_TABLE->query_enemies(al_me, al_him))
  {
    return "%^BOLD%^RED%^";
  }
  else if (pov_name == POV_DIFFICULTY)
  {
    int dif;

    dif = he->query_level() - me->query_level();

    if (dif > 10)
      return "%^BOLD%^RED%^";
    else if (dif > 5)
      return "%^BOLD%^ORANGE%^";
    else if (dif > -2)
      return "%^GREEN%^";
  }

  return "";
}

string query_name_changed(object me, object he)
{
  object race;

  if (!me || !he || (me == he))
    return "";

  // with npcs of players without race (can this happen?) do not show the race
  if (!he->query_race_ob() || he->query_npc())
    return capitalize(he->query_cap_name());

  // is a player with race_ob

  race = load_object(he->query_race_ob());

  return capitalize(he->query_cap_name()) + " " + 
          race->query_race_gender_string(he);
}
