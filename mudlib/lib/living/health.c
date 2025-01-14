
#include <common/properties.h>
#include <living/food.h>
#include <living/combat.h>
#include <common/quests.h>
#include <mud/secure.h>
#include <user/xp.h>
#include <language.h>

int max_hp, max_gp, total_xp, wimpy;
int hp, xp, gp;

private int * drink_info;
private int display_monitor_handle;

static mapping damage_done;
static mapping aggro_done;

int headache;

void create() 
{
  damage_done = ([ ]);
  aggro_done = ([ ]);
  drink_info = allocate_int(D_SIZEOF);
  max_gp = 1;
  max_hp = 1;
  display_monitor_handle = 0;
  headache = 0;
}

int query_volume(int type);
void update_volumes();

void heart_beat()
{
  int intox;

  intox = query_volume(D_ALCOHOL);

  // TODO -> review drunk.c
  // if (drunk_heart_beat(intox) > 0)

  // handle intoxication dispersion by our selves...
  // they just handle hp recival and sp recival... 
  if (headache)
    if (!--headache) 
    {
      tell_object(this_object(), _LANG_HEALTH_DRUNK_PASSES);
      headache = 0;
    }

  if (intox) 
  {
    if (!(intox-1)) 
    {
      headache = 15;
      tell_object(this_object(), _LANG_HEALTH_DRUNK_STARTS);
      hp -= 2;
      if (hp < 1)
        hp = 1;
    }
  }

  update_volumes();
}

int query_hp() { return hp; }
int query_max_hp() { return max_hp; }
int query_gp() { return gp; }
int query_max_gp() { return max_gp; }
int query_xp() { return xp; }
int query_total_xp() { return total_xp; }
mapping query_damage_done() { return damage_done; }
mapping query_aggro_done() { return aggro_done; }

int set_wimpy(int i)
{
  int j;
  j = this_object()->query_property(FEAR_PROP);

  if (i < j) 
    i = j;
  if (i <= 0)
    i = 0;
  if (i >= 100)
    i = 99;
  
  return (wimpy = i);
}

int query_wimpy() { return wimpy; }

int set_max_hp(int i) 
{
  if (max_hp)
    hp = hp*i/max_hp;
  else
    hp = max_hp;
  max_hp = i;
  if (max_hp==1)
    max_hp = 2;
  if (hp>max_hp)
    hp = max_hp;
  return max_hp;
}

// Made varargs : Radix - March 3, 1997
int set_hp(int i, varargs object hp_remover)
{
  if (i > max_hp)
    i = max_hp;
  hp = i;
  if (hp < 0)
    call_out("do_death", 0,  hp_remover);
  return hp;
}

void end_practise(object loser, object winner)
{
  tell_player(loser, _LANG_HEALTH_PRACTICE_LOSER);
  tell_player(winner, _LANG_HEALTH_PRACTICE_WINNER);
  
  loser->stop_fight(winner);
  winner->stop_fight(loser);    
  
  // 5 hbs == 10 seconds
  loser->add_timed_property(PASSED_OUT_PROP, _LANG_HEALTH_PRACTICE_PASSED_PROP, 5);
}

int adjust_aggro(int i, object aggro_doer)
{
  if (aggro_doer && !(int)aggro_doer->query_dead() )
  {
    aggro_done[aggro_doer] += i;
  } 

  return aggro_done[aggro_doer];
}

object query_main_aggro_doer()
{
  object doer;
  object * atts;
  int i, max;
  
  doer = nil;
  atts = this_object()->query_attacker_list();
  max = 0; // aggro values are negative!

  for (i = 0; i < sizeof(atts); i++)
  {
    if (aggro_done[atts[i]] && (aggro_done[atts[i]] < max))
    {
      doer = atts[i];
      max = aggro_done[atts[i]];
    }
  }

  // in case they are starting combat
  if (sizeof(atts) && !doer)
    doer = atts[0];

  return doer;
}

int adjust_hp(int i, varargs object hp_remover)
{
  int color;
  hp += i;

  color = 1; // for color on hp monitor - Radix

  if (hp_remover && !(int)hp_remover->query_dead())
  {
    if (damage_done[hp_remover])
      damage_done[hp_remover] += i;
    else
      damage_done[hp_remover] = i;
    
    if (aggro_done[hp_remover])
      aggro_done[hp_remover] += i;
    else
      aggro_done[hp_remover] = i;
  }

  if (hp > max_hp)
    hp = max_hp;

  if (hp < 0) /* eeek we died! */
  {
    // safe combat, we finish the practice
    if ( // (sizeof(this_object()->query_attacker_list()) == 1) &&  
      hp_remover && (hp_remover->query_combat_mode() == SAFE_COMBAT))
    {
      hp = 1;
      this_object()->stop_fight(hp_remover);
      hp_remover->stop_fight(this_object());
      call_out("end_practise", 0, this_object(), hp_remover);
      
      // quests of winning in combat (non lethal)
      if (hp_remover->is_doing_quest(base_name(this_object()), TYPE_BEAT) )
        hp_remover->update_quest(base_name(this_object()));
      else if (hp_remover->is_doing_quest_from_name(this_object()->query_short(), TYPE_BEAT) )
        hp_remover->update_quest_from_name(this_object()->query_short());
    }
    else // non safe combat, they die
    {
      if (i > 0 && hp_remover == this_object()) 
        call_out("do_death", 0, nil); // NOT this object, use attacker list
      else
        call_out("do_death", 0, hp_remover);
    }   
  }

  // Radix Jan 1996
  if (hp < max_hp/5)
    color = 0;

  if (interactive(this_object()))
  {
    if (display_monitor_handle)
      remove_call_out(display_monitor_handle);
    display_monitor_handle = call_out("display_monitor", 0, color);
  }
  
  return hp;
}

int set_max_gp(int i) 
{
  if (max_gp)
    gp = gp * i / max_gp;
  else
    gp = max_gp;

  max_gp = i;

  if (gp > max_gp)
    gp = max_gp;

  return max_gp;
}

int set_gp(int i) 
{
  gp = i;
  if (gp > max_gp)
    gp = max_gp;
  return gp;
}

int adjust_gp(int i) 
{
  if (gp + i < 0)
    return -1;

  gp += i;

  if (gp > max_gp)
    gp = max_gp;

  return gp;
}

void logit(string what, int amount)
{
  if (interactive(this_object()))
  {
    log_file(LOG_XP, "["+ctime(time(),4)+"] " + (string)this_object()->query_cap_name()+" got "+amount+" "+what+
      " from: "+previous_object()->query_name()+", file: "+file_name(previous_object())+
      "\n");
  }
}

// new auto-adjust level advancing system, neverbot 7/03
int adjust_xp(int i)
{
  object ob;
  int next_level;

  if (!SECURE->valid_adjust_xp(previous_object(), this_object(), i))
    return xp;

  if (i > XP_LIMIT)
  { 
    logit("xp", i);
    i = XP_LIMIT;
  }

  if (i > 0) 
  {
    i = (100 - wimpy) * i / 100;
    total_xp += i;
  }

  xp += i;

  /* 
  // changed, now guild advancing is via quest
  // neverbot 06/2010
  
  ob = this_object()->query_guild_ob();
  if (ob && (i > 0)) 
  {
    next_level = ob->query_next_level_xp(this_object());
    if ((xp > next_level) && 
      (this_object()->query_guild_level() < ob->query_max_level()) )
    {
      this_object()->adjust_guild_level(1);
      xp -= next_level;
      this_object()->save();
    }
  }
  */

  ob = load_object(this_object()->query_class_ob());
  if (ob && (i > 0)) 
  {
    next_level = ob->query_next_level_xp(this_object());
    if ((xp > next_level) && 
      (this_object()->query_class_level() < ob->query_max_level()) )
    {
      this_object()->adjust_level(1);
      xp -= next_level;
      this_object()->save();
    }
  }

  return xp;
}

// int set_xp(int i) { xp = i; return xp; }
int set_xp(int i) 
{
  // Radix : Aug 1997
  if (!SECURE->valid_adjust_xp(previous_object(), this_object(), i))
    return xp;

  if ( (i-xp) > XP_LIMIT) 
    logit("xp",(i-xp));

  if ( (xp-i) > XP_LIMIT) 
    logit("xp", -(xp-i));

  if (i == -1 || !i) 
    logit("xp", i);

  xp = i;
  return xp;
}

// self added, neverbot 02/2006
string health_string(int self) 
{
  string ret;
  
  if (hp < max_hp/20)
    ret = _LANG_HEALTH_DEATH_THRESHOLD;
  else if (hp < max_hp/10)
    ret = _LANG_HEALTH_CRITICAL_THRESHOLD;
  else if (hp < max_hp/5)
    ret = _LANG_HEALTH_BAD_THRESHOLD;
  else if (hp < max_hp/2)
    ret = _LANG_HEALTH_FAIR_THRESHOLD;
  else if ((float)hp < (float)max_hp/1.2)
    ret = _LANG_HEALTH_SLIGHTLY_INJURED;
  else if (hp == max_hp)
    ret = _LANG_HEALTH_PERFECT_HEALTH;
  else 
    ret = _LANG_HEALTH_GOOD_HEALTH;

  if (!self)
    return _LANG_HEALTH_THEY_ARE + " " + ret;
  else
    return _LANG_HEALTH_YOU_ARE + " " + ret;
}

// functions related to drinks
int adjust_volume(int type, int amt) 
{
  if (!pointerp(drink_info))
    drink_info = allocate_int(D_SIZEOF);
  if (type >= sizeof(drink_info))
    return 0;
  return drink_info[type] += amt;
}

int query_volume(int type) 
{
  if (!pointerp(drink_info))
    drink_info = allocate_int(D_SIZEOF);
  if (type >= sizeof(drink_info))
    return 0;
  return drink_info[type];
}

void update_volumes() 
{
  int i;

  if (!drink_info)
    return ;
  for (i = 0; i < sizeof(drink_info); i++)
    if (drink_info[i] > 0)
    drink_info[i]--;
  else
    drink_info[i] = 0;
}

string volume_string()
{
  int i;
  i = 0;

  if (drink_info[i]<= 0)
    return _LANG_HEALTH_VOLUME_SOBER;
  if (drink_info[i] <= 50)
    return _LANG_HEALTH_VOLUME_HAPPY;
  if (drink_info[i] <= 100)
    return _LANG_HEALTH_VOLUME_DRUNK;
  if (drink_info[i] <= 500)
    return _LANG_HEALTH_VOLUME_VERY_DRUNK;
  if (drink_info[i] <= 2000)
    return _LANG_HEALTH_VOLUME_DEAD_DRUNK;
  if (drink_info[i] <= 6000)
    return _LANG_HEALTH_VOLUME_NEAR_DEATH;
  return _LANG_HEALTH_VOLUME_PRAYING_FOR_DEATH;
}

mixed * stats() 
{
  return ({ ({"Max Hp", max_hp, }),
    ({"Hp", hp, }),
    ({"Max Gp", max_gp, }),
    ({"Gp", gp, }),
    ({"Xp", xp, }),
    ({"Total Xp", total_xp, }),
    ({"Wimpy", wimpy, }),
    ({"Damage Done (nosave)", damage_done, }),
    ({"Aggro Done (nosave)", aggro_done, }),
    ({"Drink Info", drink_info, }),
    });
}
