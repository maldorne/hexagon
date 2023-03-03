// Base class file...
// neverbot - 20 Junio 2003

inherit "/lib/core/object.c";

#define BASIC_XP_COST 5000
#define MAX_LEVEL 20

mixed * class_commands;
string * legal_races;
// Bonus al incremento de gps y hps en cada nivel
int hp_bonus, gp_bonus;
// Tipo de dado que se lanza para subir gps y hps
int hit_dice, gp_dice;

// Define la facilidad para el combate de una clase
// (este bono se añade a la AC del pj)
int combat_bonus;

// Sistema de tipos de xp, neverbot 07/04
// tipos definidos en /include/xp_types.h
// Define tipo_de_xp:porcentaje
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

// Para comprobar razas
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

// A la hora de subir niveles comprobamos la caracteristica
//  que nos proporciona gps (esta funcion debe enmascararse en cada
//  clase concreta
int query_gp_main_stat(object player){
  if (player)
    return player->query_str();
}

// Añadido 7/03, neverbot (mismo sistema que gremios)
/* What happens when you advance in level */
void new_levels(int lvls, object ob) {
  ob->reset_all();
  ob->recalc_stats(lvls);
}

// Nuevo sistema de subida automatica de niveles de clase
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

// Sistema de tipos de xp, neverbot 07/04
mapping query_xp_types(){ return xp_types; }
void set_xp_types(mapping types){
   xp_types += types;
}
