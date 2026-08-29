// Base class file...
// neverbot - 20 June 2003

inherit "/lib/core/object.c";

#define BASIC_XP_COST 5000
#define MAX_LEVEL 20

mixed * class_commands;
string * legal_races;
// Bonus to the gp and hp increase at each level
int hp_bonus, gp_bonus;
// Type of die rolled to raise gps and hps
int hit_dice, gp_dice;

// Defines how easy combat is for a class
// (this bonus is added to the character's AC)
int combat_bonus;

// Xp types system, neverbot 07/04
// types defined in /include/xp_types.h
// Defines xp_type:percentage
// xp_types = ([ "combat":"100", "magic":"20", ]), ...
mapping xp_types;

int query_combat_bonus(){ return combat_bonus; }
void set_combat_bonus(int value){ combat_bonus = value; }
int query_hp_bonus() { return hp_bonus; }
void set_hp_bonus(int num){ hp_bonus = num; }
int query_gp_bonus() { return gp_bonus; }
void set_gp_bonus(int num){ gp_bonus = num; }
int query_hit_dice() { return hit_dice; }
void set_hit_dice(int value) { hit_dice = value; }
int query_gp_dice() { return gp_dice; }
void set_gp_dice(int value) { gp_dice = value; }

void create(){
  class_commands = ({ });
  legal_races = ({ });
  xp_types = ([ ]);
  hp_bonus = 0;
  gp_bonus = 0;
  hit_dice = 0;
  gp_dice = 0;
  ::create();
}

// To check races
string * query_legal_races() { return legal_races; }
void set_legal_races(string * list){
   legal_races = list;
   return;
}
int query_legal_race(string race){
  if (sizeof(legal_races) == 0)
    return 1;
  if (member_array(race, legal_races) == -1)
    return 0;
  return 1;
}

int query_channel() { return 0; }

int add_class_command(string name, int add_if_player) 
{
   if (member_array(name, class_commands) != -1)
      return 0;
   class_commands += ({ name, ({ add_if_player }) });
   return 1;
}

// Mask this and put your shadows in here, scope to this though!
void start_player(object pl) 
{
   int i;
   if (!pointerp(class_commands))
      class_commands = ({ });
   for (i = 0; i < sizeof(class_commands);i+=2)
      if (class_commands[i+1][0] || !interactive(pl))
         pl->add_known_command(class_commands[i]);
}

void on_death(object player, varargs object killer)
{
}

void on_kill(object player, object victim)
{
}

int player_quit(object player)  
{
    return 1;
}

// When gaining levels we check the stat that
//  provides us gps (this function must be masked in each
//  specific class)
int query_gp_main_stat(object player){
  if (player)
    return player->query_str();
}

// Added 7/03, neverbot (same system as guilds)
/* What happens when you advance in level */
void new_levels(int lvls, object ob) {
  ob->reset_all();
  ob->recalc_stats(lvls);
}

// New system for automatic class level advancement
// neverbot 16/7/03
int query_next_level_xp(object player)
{
  int res;
  res = player->query_level() * BASIC_XP_COST;
  if (res == 0) 
     return BASIC_XP_COST;
  return res;
}

int query_max_level(){
  return MAX_LEVEL;
}

// Xp types system, neverbot 07/04
mapping query_xp_types(){ return xp_types; }
void set_xp_types(mapping types){
   xp_types += types;
}
