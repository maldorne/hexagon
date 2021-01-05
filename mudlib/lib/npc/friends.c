
// code taken from monster.c, neverbot 04/2009

#include <npc/npc.h>

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

mapping  add_loved(string type, mixed targets)
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
      if (!this_object()->query_attacker_list())
      {
        if (stringp(join_fight_mess))
          tell_room(environment(this_object()),
              (string)MONSTER_HAND->expand_string(this_object(), join_fight_mess, this_object(), ob));
        else 
          tell_room(environment(this_object()),"¡" + this_object()->query_cap_name() + 
              " se une a la batalla!\n");
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
      tell_room(environment(this_object()), capitalize(this_object()->query_short()) + 
          " se une a la batalla y protege a " + ob->query_short() + ".\n");
      this_object()->attack_ob(ob1);
      this_object()->set_protecting(1);
    }
  }
}

void do_aggressive_check(object ob)
{
  if (MONSTER_HAND->do_aggressive_check(ob, aggressive,
      this_object(), minplayer, hated, loved))
  {
    if (stringp(join_fight_mess) && (join_fight_mess != "") && !this_object()->query_timed_property(NO_SPAM))
    {
      tell_room(environment(this_object()),(string)MONSTER_HAND->expand_string(this_object(),
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