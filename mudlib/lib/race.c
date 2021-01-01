/* 
 * Retocado por neverbot 3/2002 para quitar ahogos, natacion, hombres-lobo, etc 
 *
 * Adaptado por neverbot para Cc, 4/2003 
 * Eliminado gran parte del archivo, y otra gran parte movida a /std/living/death.c
 * Añadidas subrazas (o culturas) para cada raza 'generica', neverbot 6/2003
 * Ahora el sistema de Regeneracion de vida y gps tira de las razas, neverbot 7/2003
 *
 */

#include <living/races.h>
#include <translations/races.h>

inherit "/lib/core/object";

static int min_light_limit;
static int max_light_limit;

// Añadido un standard para cualquier raza, neverbot 2/2002
int body_size; // 1 menor - 5 humano - 9 mayor
// Nuevo sistema de alineamiento, neverbot 6/03
int ext_align;
// Numero de brazos
int limbs;
// Subculturas o subrazas disponibles (para buscar en el subdirectorio 'cultures')
string * cultures;
// Sistema para llevar a un personaje a su zona de inicio
string init_room;

// Nuevo para poner razas para npcs (no son jugables)
// Cualquier raza en /obj/races se hace jugable automaticamente a menos que tenga esto a 0
int is_playable;

int query_is_race_ob() { return 1; }

void create() 
{
  reset_get();

  body_size = 5; // This is used as a standard 'human' size
  limbs = 2;
  is_playable = 1;
  cultures = ({ });
  init_room = "";
  // Alineamiento por defecto neutral   
  ext_align = random(100);

  ::create();
  // set_name("criatura desconocida");
  // set_long("La raza estandar de "+mud_name()+", debes hacerte miembro "
  //  "de una nueva raza pronto, esta es muy aburrida.\n");
  // set_light_limits(LSTANDARDL, LSTANDARDH);
}

// Dummy for now, allows death curses etc.
// taniwha 1996
void on_death(object player, object killer) { }
void on_kill(object player, object victim) { }

int query_playable() { return is_playable; }
void set_playable(int p)
{ 
  if (p > 0) 
    is_playable = 1; 
  else
    is_playable = 0; 
}

int set_light_limits(int lower, int upper)
{
  min_light_limit = lower;
  max_light_limit = upper;
  if (lower < L_B_L)
    min_light_limit = L_B_L;
  if (upper>L_B_H)
    max_light_limit = L_B_H;
  return 1;
}

int query_light_limith() { return max_light_limit; }
int query_light_limitl() { return min_light_limit; }

void set_body_size(int i) { body_size = i; }
int query_body_size() { return body_size; }

string * query_cultures(){ return cultures; }
void set_cultures(string * list){ cultures = list; }

string query_init_room(){ return init_room; }
void set_init_room(string where){ init_room = where; }

/* armor querying one... I forget what it is called */
int query_number_type(string type)
{
  if (type == "ring")
    return 2;
  return 1;
}

/* number of weapons allowed to be wielded */
/*
int query_number_wielded() {
  return limbs;
  // return 1;
}
*/

/* the number of hands the race has */
void set_limbs(int i) { limbs = i; }
int query_limbs() { return limbs; }

/* race description used on the player object...
 * There will be a race varible on the player object which allows
 * the race to keep needed information on it
 */
string query_desc(object ob) 
{ 
  return _LANG_RACES_UNKNOWN_DESC; 
}

/* when the player logs on this function is called.
 * it allows you to start up the player with whatever he needs.
 * EXCEPT it's not used, see start_player() below *curses*  
 */
/*
int player_start(object player) {
  return 1;
}
*/

/* and guess what this does?
 * Beware... If some is straight dested this will not be called,
 * So remember that you still must check your array for null members.
 */
int player_quit(object player)
{
  return 1;
}

int query_dark(int i)
{
  if (i < (min_light_limit - 5) ) return 1;  /* Can't see a thing */
  if (i <  min_light_limit) return 2;        /* can sorta see but not much */
  if (i < (min_light_limit + 5) ) return 3;  /* starting to lose it */
  if (i > (max_light_limit + 40) ) return 6; /* blinded by the light */
  if (i > (max_light_limit + 20) ) return 5; /* dazzled */
  if (i >  max_light_limit) return 4;        /* squinting */
  return 0;
}

int query_max_level(string guild) { return 100; }

// new alignment system, neverbot 6/03
int query_ext_align() { return ext_align; }
void set_ext_align(int i) { ext_align = i; }

void set_racial_bonuses(object ob) { }

void start_player(object ob)
{
  ob->add_alias(lower_case(this_object()->query_name()));
}

void race_heartbeat(object player)
{
  // if (!player) return;
  // if (!environment(player)) return;
  // if (player->query_dead()) return;
}

// functions added to customize, neverbot 4/2003
string * query_initial_languages()
{
  return ({ });
}

int query_race_weight()
{
  return STD_WEIGHT;
}

// body location system based in an original idea by Iolo@Rl, changed and updated by
// neverbot for CcMud, 4/2003
// By default in any race the locations are of a humanoid body
string * query_locations()
{
  return ({ _LANG_RACES_LOCATIONS_HEAD, _LANG_RACES_LOCATIONS_CHEST, 
    _LANG_RACES_LOCATIONS_ARM, _LANG_RACES_LOCATIONS_LEG,
    _LANG_RACES_LOCATIONS_HAND, _LANG_RACES_LOCATIONS_FOOT });

}

// returns enough info to hit in a body location
mixed obtain_location()
{
  float mult; // damage multiplier
  string name; // location name

  switch (random(30)) 
  {
    case 0:      mult = 2.0;  name = _LANG_RACES_LOCATIONS_HEAD;  break;
    case 1..4:   mult = 1.5;  name = _LANG_RACES_LOCATIONS_CHEST; break;
    case 5..14:  mult = 1.0;  name = _LANG_RACES_LOCATIONS_ARM;   break;
    case 15..24: mult = 1.0;  name = _LANG_RACES_LOCATIONS_LEG;   break;
    case 25..28: mult = 0.5;  name = _LANG_RACES_LOCATIONS_HAND;  break;
    case 29:     mult = 0.5;  name = _LANG_RACES_LOCATIONS_FOOT;  break;
  }

  // damage multiplier, location name, body type
  return ({ mult, name, _LANG_RACES_HUMANOID_BODY });
}

// Añadidos por neverbot 4/03, para eliminar el gender_table.c
string query_race_gender_string(object player, varargs int flag)
{
  if (!flag)
  {
    if (player->query_gender()==2)
       return "la esencia";
     return "el ente";
    }
    else{
    if (player->query_gender()==2)
       return "esencia";
     return "ente";
    }
}

/* Nuevo sistema de regeneracion de vida/gps
 *
 * Estas funciones reciben el objeto player y
 *  comprueban si deben o no regenerar pvs!!
 * Para razas como la drow se puede comprobar si el
 *  jugador esta en underground, si es dia/noche, etc
 * (por defecto siempre se suben gps y pvs)
 */

int query_regen_hp(object player) { return 1; }
int query_regen_gp(object player) { return 1; }
