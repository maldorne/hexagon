/*
 * Baldrick, Wonderflug, Taniwha 1996, moved "brains" to handler object.
 * Not for effiency, just to allow more flexibility.
 *
 * Revision para CcMud, neverbot 6/03
 *
 *
 * Extraidas funcionalidades de loved/hated/aggressive, etc
 *   a /global/npc/friends.c
 * Extraidas funcionalidades de chat y talk
 *   a /global/npc/chatter.c
 * neverbot 04/2009
 */

#include <areas/weather.h>
#include <npc/npc.h>
#include <living/living.h>

inherit living     "/lib/living/living.c";
inherit friends    "/lib/npc/friends.c";
inherit chatter    "/lib/npc/chatter.c";
inherit npc_combat "/lib/npc/npc_combat.c";
inherit npc_timed  "/lib/npc/npc_timed.c";

// already defined in living/combat.c
// static int combat_counter;  /* Counts what we should do next in combat */

// Taniwha 1995
int p_memory;          /* if !0 will record player names, else won't */
string *p_attack_list; /* names we don't like */

mixed move_after;      /* ({ minimum time, Add'l random time }) */

int move_when;         /* how many hbs we shall move */

string *move_zones,    /* zones that i may wander into */
*enter_commands;       /* Commands to be done upon entering a room */

static int hbcheck;    /* A little patch to avoid slaughtering cowards */
static int only_one;   /* Sólo puede clonarse si es el unico que existe
                en el mud (no puede haber dos) */

static int _follow_move_handle;
/*** End of local variables ***/

nomask int query_player() { return 0; }
int query_concentrate_valid() { return 1; }
int query_npc() { return 1; }
int query_monster() { return 1; }
nomask int query_coder() { return 0; }

// Para el sistema de guardianes de salidas
int guardian_check(object ob) { return 1; }

void create()
{
  int time, i;
  object * obs;
  string aux;

  reset_get();

  p_attack_list = ({ });
  move_zones = ({ });
  enter_commands = ({ });
  only_one = 0;

  add_language("comun");
  set_language("comun");

  friends::create();
  chatter::create();
  npc_combat::create();
  npc_timed::create();
  living::create();

  if (!this_object()->query_race())
    set_race_ob("/obj/races/desconocida");

  combat_counter = 0;

  if (this_object()->query_weight() == 0)
    set_weight(1500);

  // Si debe ser único y hay más, tenemos que destruirlo
  if (only_one)
  {
    // obtenemos el nombre de archivo de este objeto
    aux = base_name(this_object());

    // Comprobamos cuantos hay clonados
    obs = children(aux);

    if (sizeof(obs))
      for (i = 0; i < sizeof(obs); i++)
      {
        // De la lista de clonados eliminamos el objeto en memoria (si lo hay)
        // (el que no tiene # en el nombre)
        if (strsrch(file_name(obs[i]), "#") == -1)
            obs -= ({ obs[i] });
      }

    // Llegados a este punto este objeto ya responde a children (al menos habra uno
    // en la lista... si hay mas no podemos clonar)
    if (sizeof(obs) > 1)
    {
      // this_object()->dest_me();
      destruct(this_object());
      return;
    }
  }

  // ****************************** Timed npcs, para mas informacion, /global/npc/npc_timed.c

  // Una vez aqui ya hemos ejecutado el setup del npc
  //  (is_timed estara actualizado!!)
  // Añadido: Solo hacemos el dest si el nombre del objeto lleva '#'
  //  en otro caso solo lo estamos cargando en memoria (update por ejemplo).

  if (is_timed && this_object() && (  strsrch(file_name(this_object()), "#") != -1 ) )
  {
    time = handler(WEATHER_HANDLER)->query_date_data()[0];
    // Si no es su hora no clonamos el npc
    if ((time < init_hour) || (time > end_hour))
        dest_me();
    return;
  }

  if (is_night_timed && this_object() && (  strsrch(file_name(this_object()), "#") != -1 ) )
  {
    time = handler(WEATHER_HANDLER)->query_date_data()[0];
    // Si no es su hora no clonamos el npc
    if ((time < init_hour) && (time > end_hour))
        dest_me();
    return;
  }

  // ****************************** Fin seccion timed

} /* create() */

// Para que sólo exista un único npc igual en el mud al mismo tiempo.
int query_only_one(){ return only_one; }
void set_only_one(int value)
{
  if (value != 0)
    only_one = 1;
  else
    only_one = 0;
}

int do_death(object killer)
{
  return ::do_death(killer);
}

// Taniwha 1996. Simply for the convenience of it
void add_clone(string what,int num)
{
  object ob;
  int i;
  if (!num) num = 1;
  for(i = 0; i < num; i++)
  {
    ob = clone_object(what);
    if (ob)
    {
      if (ob->move(this_object()))
      {
        if (ob && environment(this_object()) )
        {
          tell_room(environment(this_object()), query_cap_name()+" intenta sostener su "+ob->query_name()+
              " pero finalmente tiene que soltarlo.\n");
          ob->move(environment(this_object()));
        }
        else ob->dest_me();
      }
    }
  }
}

void attack_by(object ob)
{
  // For NPC's remember the man who shot my paw ;) Taniwha 1995
  if (p_memory && interactive(ob) &&
      (ob->query_level() > MIN_PLAYER_LEVEL) &&
      (member_array(ob->query_name(), p_attack_list) == -1) )
  {
    p_attack_list += ({ (string)ob->query_name() });
  }

  if (p_attack_list)
    do_equip("");

  ::attack_by(ob);
}

void stop_fight(object ob)
{
  if ( ob && interactive(ob) )
    p_attack_list -= ({ (string)ob->query_name() });
  ::stop_fight(ob);
}

int query_player_memory() { return p_memory; }
int set_player_memory(int i)
{
  p_memory = i;
  return p_memory;
}

void remove_p_attack_list() { p_attack_list = ({ }); }
string *query_p_attacker_list()
{
  return p_attack_list;
}

int check_anyone_here();
void set_random_stats(int low, int high);
void do_move_after(int bing);

int query_cols() { return 79; }

// This can be masked to get more limbs..
// int query_limbs()
// {
//  object ob = load_object(query_race_ob());
//  if (ob)
//      return ob->query_limbs();
//  return 2;
// }

void set_name(string n)
{
  if (name && name != "object")
    return;
  name = n;
  // add_plural(pluralize(name));
  set_short(capitalize(name));
  set_long("No puedes ver nada destacable.\n");
  set_living_name(n);
}

string long(string str, int dark)
{
  string s;
  object ob;

  s = sprintf(
    "\n  %-=*s\n",
    (this_user()?this_user()->query_cols()-2:79),
    "   " + query_long()
    );

  // neverbot, 7/03
  // Con el 1 de query_race_gender_string nos ahorramos el articulo
  // (en lugar de 'la humana', devuelve unicamente 'humana')
  if (ob = load_object(this_object()->query_race_ob()))
    if (ob->query_race_gender_string(this_object(), 1) != "")
      s += capitalize(query_pronoun())+" es "+query_numeral()+ " " +
           ob->query_race_gender_string(this_object(), 1) + ".\n";

  // s += capitalize(query_pronoun())+" "+health_string(0)+".\n";
  s += capitalize(health_string(0))+".\n";
  s += calc_extra_look();
  s += query_living_contents(0);
  return s;
}

void init_command(string str) { call_out("do_command", 0, str); }
void init_equip() { call_out("do_equip", 0, 0); }

// int do_command(string str)
// {
//   if (stringp(str))
//   {
//     // return command(str);
//     queue_action(str);
//     return 1;
//   }
//   else
//     write(printf("Parametro inválido para do_command %O en monster %O, env %O.\n", str,
//           this_object(), environment()));
// }

void init()
{
  if (!query_heart_beat())
  {
    if (check_anyone_here() || sizeof(query_attacker_list()))
      set_heart_beat(1);
  }
  // else
  // heartheal++;

  ::init();
}

void event_enter(object ob, varargs string mess, object from, mixed avoid)
{
  if (!ob)
    return;

  // Taniwha 1995, I remember you you bastard, you attacked me then logged off
  if (interactive(ob) && member_array(ob->query_name(), p_attack_list) != -1)
  {
    attack_by(ob);
    // This should avoid problems with players entering, attacking and repeat
    // yeah, I removed it, taniwha
    // {
    attack(); // Taniwha 1995.
    hbcheck++;
    // }
    return;
  }

  do_aggressive_check(ob);
}

int check_anyone_here()
{
  object *arr;
  int i;

  if (!environment())
    return 0;
  arr = all_inventory(environment());
  arr -= ({ this_object() });

  for (i = 0; i < sizeof(arr); i++)
    // El heart_beat debe continuar tambien si hay otros npcs en la room
      // if (arr[i] && interactive(arr[i]))
    if (arr[i] && living(arr[i]))
      return 1;
  return 0;
}

void set_move_after(int after, int rand)
{
  move_after = ({ after, rand });
  move_when = (move_after[0] + random(move_after[1]));
}

void run_away(varargs int bing)
{
  if (bing)
    do_move_after(bing+1);
  else
    do_move_after(1);
}

int query_hb_counter() { return hb_counter; }
int query_hb_diff(int oldv)
{
  if (hb_counter > oldv)
    return (hb_counter - oldv);
  else
    return (oldv - hb_counter);
}

/* More heart_beat stuff, removing pieces from void heart_beat to make the
 * function less complex.
 * First go is the start and stop of heartbeats.
 * Baldrick, march '95
 */
int check_heart_beat()
{
  if ( hp == max_hp && gp == max_gp &&
    !sizeof(query_attacker_list()) &&
    !this_object()->query_action_pending() &&
    !check_anyone_here() && !sizeof(query_effects()) &&
    !this_object()->ai_actions_pending())
  {
    set_heart_beat(0);
    protecting = 0;
  }
}

/* Does the move after thingie called in heart_beat
* Wonderflug, cut this down to nothing :)
*/
void move_after_heart_beat()
{
  // Taniwha 07 02 97, can't handle the crap uptimes.
#ifdef NO_WANDER
  return;
#endif
  move_when--;
  if ( move_when <= 0 )
  {
    do_move_after(0); // probably change that number.. random ?
    move_when = (move_after[0] + random(move_after[1]) );
  }
  return;
} /* void move_after */

/*
 * This code is *ugly* and it has to be possible to make it cleaner and faster!
 *   Hrmf..
 * Well, actually done that now, moved a lot of the stuff in it to their
 *   own funtions (right above), they are as ugly as before, but it looks a bit
 *   cleaner.
 *   Baldrick. march '95
 */
void heart_beat()
{
  hbcheck = 0; // Taniwha, so they don't attack TOO often

  living::heart_beat();

  /* This is the attack part.
   * it runs when the monsie is in fight.
   */
  if ( (sizeof(attacker_list) || sizeof(call_outed)) && sizeof(query_effects()) )
  {
    MONSTER_HAND->effect_heart_beat(this_object(), attacker_list, query_attack_effects());
    chatter(achat_chance, achat_string);
  }
  else
  {
    chatter(chat_chance, chat_string);
  }

  if (move_after && !sizeof(attacker_list))
    move_after_heart_beat();

  /* No race objects have this.
   * catch(query_race_ob()->monster_heart_beat(race, mclass, query_race_ob(), query_guild_ob()));
   */

  /* This will start and stop the heartbeats dependant on a lot of
   * variables. this is to lessen the cpu.
   */
  check_heart_beat();
} /* void heart_beat */

void set_random_stats(int low, int max)
{
  set_str(random(low, max));
  set_dex(random(low, max));
  set_int(random(low, max));
  set_con(random(low, max));
  set_wis(random(low, max));
  set_cha(random(low, max));
  set_wil(random(low, max));
  set_per(random(low, max));
}

int set_guild_level(int i)
{
  if (i < 1)
    i = 1;

  adjust_guild_level(i - query_guild_level());
  return query_guild_level();
}

int set_level(int i)
{
  if (i > MIN_MONSTER_LEVEL)
    p_memory = 1;
  if (i < 1)
    i = 1;

  adjust_class_level(i - query_class_level());

  set_max_hp(query_con() * query_level());
  set_hp(query_max_hp(), this_object());

  set_max_gp(query_gp_main_stat() * query_level());
  set_gp(query_max_gp());

  // Elimina bonos temporales a stats, bonificadores y resistencias (solo las temporales!!)
  reset_all();

  // Ranking de jugadores, tambien para npcs, neverbot 06/10
  // Solo para npcs unicos
  if (query_only_one())
    call_out("check_ranking", 5);

  return query_class_level();
}

// Funcion que añade al npc al ranking de niveles mas altos
void check_ranking()
{
  object ranking;

  // Solo objetos clonados
  if (!environment(this_object()))
    return;

  ranking = load_object(RANKING_OB);

  if (ranking)
    ranking->update(this_object());
}

int set_class_level(int i) { return set_level(i); }

void add_move_zone(mixed zone)
{
  int i;

  if (pointerp(zone))
    for (i=0;i<sizeof(zone);i++)
      add_move_zone(zone[i]);
  else if (member_array(zone, move_zones) != -1)
    return;
  else
    move_zones += ({ zone });

  if (!move_after)
    set_move_after(MOVE_DEFAULT[0], MOVE_DEFAULT[1]);
}

string *query_move_zones() { return move_zones; }
void set_move_zones(string *zones)
{
  int i;

  for (i = 0; i < sizeof(zones); i++)
  add_move_zone(zones[i]);
}

void remove_move_zone(mixed zone)
{
  int i;

  if (pointerp(zone))
    for (i = 0; i < sizeof(zone); i++)
      remove_move_zone(zone[i]);
  else
  {
    if ((i = member_array(zone, move_zones)) == -1)
      return;
    move_zones = delete(move_zones, i, 1);
  }

  if ( !sizeof(move_zones) )
    move_after = 0;
}

string *query_enter_commands() { return enter_commands; }
int add_enter_commands(mixed str)
{
  if (stringp(str))
    enter_commands += ({ str });
  else if (pointerp(str))
    enter_commands += str;
  return 1;
}

int reset_enter_commands()
{
  enter_commands = ({ });
}

int move_living(string dir, string dest, mixed message, mixed enter)
{
  int i, j;

  i = ::move_living(dir, dest, message, enter);
  protecting = 0;

  if (i)
  {
    for (j = 0; j < sizeof(enter_commands); j++)
      init_command(enter_commands[j]);
    return i;
  }

  return 0;
}

mixed query_move_after() { return move_after; }
void do_move_after(int bing)
{
  mixed *direcs, *direcstemp;
  int i, dd, bong;
  string zone;

  if (!environment(this_object()))
  {
    call_out("dest_me",0);
    return;
  }

  direcstemp = (string *)environment()->query_dest_dir();
  direcs = ({});
  for(dd=0;dd<sizeof(direcstemp);dd+=2)
  {
    if (find_object(direcstemp[dd+1]))
    {
      direcs += direcstemp[dd..dd+1];
    }
  }
  while (!bong && sizeof(direcs))
  {
    i = random(sizeof(direcs)/2)*2;

    bong = 0;
    if (bing > 1)
      catch(bong = (int)direcs[i+1]->query_property("no throw out"));
    catch(zone = (string)direcs[i+1]->query_zone());
    if (sizeof(move_zones) || bong)
      if (bong || member_array(zone, move_zones) == -1)
      {
        direcs = delete(direcs, i, 2);
        continue;
      }

    queue_action(direcs[i]);
    bong = 1;
    if (!bong)
        direcs = delete(direcs, i, 2);
  } /* while */
} /* do_move_after() */

// Added by Wonderflug.  To fix add_protect.
void event_death(object caller, varargs object killer,
                                        object * attackers,
                                        mixed avoid)
{
  if ( caller == this_object() )
    return;
  else
    protecting = 0;
}

void event_fight_in_progress(object one, object two)
{
  if (join_fights)
  {
    attacker_list -= ({ 0 });
    pile_in(one,two);
    pile_in(two,one);
  }
}

void event_exit(object ob, varargs string mess, object dest, mixed avoid)
// void event_exit(object ob, string mess, mixed dest)
{
  mixed *bing;
  int i;
  string zone, file;

  if (!move_after) // we dont move :(
    return;

  // follow the suckers.
  if (member_array(ob, attacker_list) != -1)
  {
    bing = (mixed *)environment()->query_dest_dir();

    // if ((i = member_array(dest, bing)) == -1)
    // {
      // if (!objectp(dest))
      //   dest = find_object(dest); // arhghh must have teleported... scums.
      // else
      file = file_name(dest);
      if ((i = member_array(file, bing)) == -1)
        return; // lost cause
    // }

    zone = (string)dest->query_move_zone();

    if (move_zones && sizeof(move_zones))
      if (member_array(zone, move_zones) == -1)
      {
        return; /* If we are suppose to wander in move_zones.. dont go
                  * where we are not supposed to... get stuck */
      }

    remove_call_out(_follow_move_handle);
    _follow_move_handle = call_out("do_follow_move", 2 + random(query_follow_speed()), bing[i-1]);
  }
}

string expand_nickname(string str) { return str; }

/*
// Added by Asmodean to make em go away like good monies
// Its 2:30am, give me a break.
int clean_up(int used)
{
return ::clean_up(used);
}
*/

void dest_me()
{
  // Extraemos el objeto de weather.c
  if (query_timed_npc())
  {
    handler(WEATHER_HANDLER)->unnotify_me(this_object());
    // destruct(this_object());
  }

  ::dest_me();
}

mixed *stats()
{
  mixed * zone;
  mixed * ret;
  int i;

  zone = ({ });

  for (i = 0; i < sizeof(move_zones); i++)
    zone += ({ ({ "Move zone "+i, move_zones[i] }) });

  ret = living::stats() + zone;

  if (query_move_after())
      ret += ({
          ({ "Move After-fix", query_move_after()[0] }),
          ({ "Move After-rand", query_move_after()[1] }),
        });

  return ret + friends::stats() + chatter::stats() +
      npc_combat::stats() + npc_timed::stats();
}
