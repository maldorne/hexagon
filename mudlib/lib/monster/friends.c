
// code taken from monster.c, neverbot 04/2009

#include <npc/npc.h>
#include <common/properties.h>
#include <language.h>

mapping loved;          /* How do I love thee, let me count the ways */
mapping hated;          /* ditto hated */
string join_fight_mess; /* I shall say this when I join a fight */
int join_fights,        /* Do we join into fights in the room ? */
    fight_npcs,         /* fight other NPC's as well as players? */
    aggressive;         /* level of aggressiveness, >do_aggressive_check*/
    
int minplayer;          /* minimum player level an NPC will attack, extra param to set_aggressive(), default 0 */

void create()
{
  loved = ([ ]);
  hated = ([ ]);
  
  join_fight_mess = "";
  join_fights = 0;
  fight_npcs = 0;  
  minplayer = 0;
  aggressive = 0;
}

int query_join_fights() { return join_fights; }
void set_join_fights(varargs int npcs)
{
  join_fights = 1;
  fight_npcs = npcs;
}

string query_join_fight_mess() { return join_fight_mess; }
void set_join_fight_mess(string str)
{ 
	join_fight_mess = str;
	set_join_fights(1);
}

mapping add_hated(string type, mixed targets)
{
  hated[type] = targets;
  return hated;
}

mapping add_loved(string type, mixed targets)
{
  loved[type] = targets;
  return loved;
}

void pile_in(object ob, object ob1)
{
  if (ob != this_object() && (interactive(ob) || fight_npcs))
  {
    if (!this_object()->query_attacker_list() ||
        member_array(ob, this_object()->query_attacker_list()) == -1)
    {
      if (!sizeof(this_object()->query_attacker_list()))
      {
        if (stringp(join_fight_mess))
          tell_room(environment(this_object()),
              (string)this_object()->chat_expand_string(this_object(), join_fight_mess, this_object(), ob));
        else 
          tell_room(environment(this_object()), _LANG_FRIENDS_PILE_IN);
      }

      this_object()->attack_ob(ob);
    }
  }

  // join the fight protecting somebody
  if (ob != this_object() && ob1 != this_object() &&
      sizeof(this_object()->query_protect()) && !this_object()->query_protecting() )
  {
    if (!interactive(ob) && member_array(ob->query_name(), this_object()->query_protect()) != -1)
    {
      tell_room(environment(this_object()), _LANG_FRIENDS_PILE_IN_PROTECTING);
      this_object()->attack_ob(ob1);
      this_object()->set_protecting(1);
    }
  }
}

// Whether somebody fits a description of livings, the shape both the loved and
// the hated lists are written in: a race, a guild, a group, a race group, a
// name, a deity, a citizenship, or simply a property they carry. Each key holds
// one value or a list of them, and any match is enough.
// Taniwha, util routine for "membership"
int matches_group(mapping description, object who)
{
  mixed list;
  int i;

  list = description["race"];
  if (pointerp(list) &&
    ((member_array(who->query_race_name(), list) != -1) ||
     (member_array(who->query_base_race_name(), list) != -1)) )
    return 1;
  if (stringp(list) && (string)list == (string)who->query_race_name()) return 1;
  if (stringp(list) && (string)list == (string)who->query_base_race_name()) return 1;

  list = description["guild"];
  if (pointerp(list) && member_array(who->query_guild_name(), list) != -1) return 1;
  if (stringp(list) && (string)list == (string)who->query_guild_name()) return 1;

  list = description["group"];
  if (pointerp(list) && member_array(who->query_group_name(), list) != -1) return 1;
  if (stringp(list) && (string)list == (string)who->query_group_name()) return 1;

  list = description["race_group"];
  if (pointerp(list) && member_array(who->query_race_group_name(), list) != -1) return 1;
  if (stringp(list) && (string)list == (string)who->query_race_group_name()) return 1;

  list = description["player"];
  if (pointerp(list) && member_array(who->query_name(), list) != -1) return 1;
  if (stringp(list) && (string)list == (string)who->query_name()) return 1;

  list = description["deity"];
  if (pointerp(list) && member_array(who->query_deity_name(), list) != -1) return 1;
  if (stringp(list) && (string)list == (string)who->query_deity_name()) return 1;

  list = description["city"];
  if (pointerp(list) && member_array(who->query_city_name(), list) != -1) return 1;
  if (stringp(list) && (string)list == (string)who->query_city_name()) return 1;

  list = description["property"];
  if (pointerp(list))
    for (i = 0; i < sizeof(list); i++)
      if (stringp(list[i]) && who->query_property(list[i])) return 1;
  if (stringp(list) && who->query_property(list)) return 1;

  return 0;
}

// Whether this monster would attack somebody who just turned up. The levels of
// aggressiveness read the loved and the hated lists differently: the lowest
// only goes for the hated, and the highest for anybody who is not loved,
// players and NPCs alike, hidden or not.
// some changes neverbot, 6/03
private int would_attack(object ob)
{
  if (!ob) return 0;
  if (ob->query_invis()) return 0;
  if (!ob->query_alive()) return 0;
  if (ob->query_level() < minplayer) return 0;
  if (ob->query_timed_property(PASSED_OUT_PROP)) return 0;

  switch (aggressive)
  {
    case 0:
      if (!interactive(ob)) return 0;
      if (ob->query_hidden()) return 0;
      if (mappingp(hated) && matches_group(hated, ob)) return 1;
      return 0;

    case 1:
      if (mappingp(loved) && matches_group(loved, ob)) return 0;
      if (!interactive(ob)) return 0;
      if (ob->query_hidden()) return 0;
      return 1;

    case 2:
      if (mappingp(loved) && matches_group(loved, ob)) return 0;
      if (ob->query_hidden()) return 0;
      return 1;

    case 3:
      if (mappingp(loved) && matches_group(loved, ob)) return 0;
      if (!interactive(ob)) return 0;
      return 1;

    case 4:
      if (mappingp(loved) && matches_group(loved, ob)) return 0;
      return 1;
  }

  return 0;
}

void do_aggressive_check(object ob)
{
  if (would_attack(ob))
  {
    if (stringp(join_fight_mess) && (join_fight_mess != "") && !this_object()->query_timed_property(NO_SPAM))
    {
      tell_room(environment(this_object()),(string)this_object()->chat_expand_string(this_object(),
                                        join_fight_mess, this_object(), ob));
      this_object()->add_timed_property(NO_SPAM, 1, 10);
    }
    this_object()->attack_ob(ob);
  }
}

int query_aggressive() { return aggressive; }
void set_aggressive(int a, varargs int level)
{
  aggressive = a;
  if (level)
    minplayer = level;
  else
    minplayer = 0;
}
    
mixed * stats() 
{
  mixed * ret;

  ret = ({
    ({ "Join Fights", join_fights }),
    ({ "Join Fight Mess", join_fight_mess }),
    ({ "Fight Npcs", fight_npcs }),
    ({ "Loved", loved }),
    ({ "Hated", hated }),
    ({ "Minplayer", minplayer }),
    ({ "Aggressive", aggressive }),
          });

    return ret;
}