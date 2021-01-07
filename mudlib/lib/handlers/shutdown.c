/* 
 * Current version 4.0 for Final Realms, your god Baldrick
 * Bashed for our great mud by the great Pinkfish, blue of Discworld
 * Many improvements from orignal version.
 * Cleaned up a little - Baldrick, April 1995
 *     Removed transporting of players, we have auto_load
 * No earmuffed shouts if below 2 minutes - Radix, January 1996
 * Auto reboots fail if dested, hopefully fixed - Radix, August 1996
 * Updated for CcMud, neverbot 4/2003
 * Now saving the weather handler, which controls the in-game time 
 *     advancement, neverbot 7/03
 */

#include <areas/weather.h>
#include <mud/shutdown.h>

inherit "/lib/core/object";

#define IMUD_SERV "/net/intermud3/services"
// #define FLAG_GAME "/d/flagomiq/handler/hand_flag"
 
int shutdown_in_progress;
int time_of_crash;
int closed;
int ishout(string str, int muff);
void do_delay_prop();

void create()
{
  ::create();
  time_of_crash = 0;
  shutdown_in_progress = 0;
}
 
void setup() 
{
  /* This stops wizzes from cloning armageddon. He only needs to be loaded */
  if (file_name(this_object()) != SHUTDOWN_OBJECT) 
  {
    write("Este objeto no puede ser clonado.\n");
    dest_me();
    return;
  }

  set_name("heraldo");
  set_living_name("heraldo");
  set_short("El Heraldo de los Dioses");
  set_long("Un magnífico ejemplar de Ángel femenino esta aquí esperando " +
           "a que los Dioses le indiquen cuando llegará una nueva Era.\n");
  add_alias("shutdown");

  move("bing");

  reset_get();
  reset_drop();
  call_out("do_delay_prop", 0);

  // Radix...
  if (this_player())
  {
     call_out("delayed_safety", 5, this_player()->query_cap_name());
     // tell_room(environment(this_object()),"¡El Heraldo comienza una cuenta atrás de 5 segundos!\n");
  }
}

/* have this move here to get it to my weather room ;) (Sleaze) */
int move(mixed dest, varargs mixed messin, mixed messout) {
  ::move(HOME, messin, messout);
}

int shutdown_activated() 
{
  return shutdown_in_progress;
}
 
// Radix was here... 
void do_delay_prop()
{
  if (environment(this_object()))
    environment(this_object())->add_property("no_clean_up",1);
}
 
// More Radix stuff...
void delayed_safety(string name)
{
  if (!time_of_crash)
  {
    log_file("game_log", "["+ctime(time())+"]" + name + 
      " cloned or reset'ed the shutdown object.\n");
    this_object()->shut(10);
  }
  return;
}
 
// Radix was here
int ishout(string str, int muff)
{
  object * all;
  int i;

  all = users();

  for (i = 0; i < sizeof(all); i++)
  {
    if (all[i])
      if (muff && all[i]->query_earmuffs()) continue;
        tell_object(all[i],"\n"+str+"\n");
  }

  return(1);
}
 
void heart_beat() 
{
  int time_to_crash;
 
  if (!time_of_crash) 
    return;

  time_to_crash = time_of_crash - time();
  
  if (time_to_crash < 1) 
  {
    call_out("end_it_all",0);
    set_heart_beat(0);
    return;
  }
   // "Gracefully" go down on intermud - Radix : Jan 5, 1996
  if (time_to_crash < 10)  
  {
    ishout("El destino del mundo se decidirá en "+time_to_crash+" segundos.",0);
    return;
  }
  if (time_to_crash < 60 && time_to_crash % 10 < 2) 
  {
    ishout("El destino del mundo se decidirá en "+time_to_crash+" segundos.",0);
    return;
  }
  if (time_to_crash % 60 > 1) 
    return;
  
  time_to_crash /= 60;
  
  if (time_to_crash < 10 || !(time_to_crash % 10)) 
  {
    if (time_to_crash > 3)
    {
      ishout("El destino del mundo se decidirá en "+time_to_crash+
        " minutos.",1);
    }
    else
      ishout("El destino del mundo se decidirá en "+time_to_crash+
        " minuto"+((time_to_crash == 1)?"":"s")+".",0);
    return;
  }
} /* heart_beat() */
 
void shut(int minutes)
{
  // string fname;
  
  if (!intp(minutes)) 
  {
    write("Argumento incorrecto.\n");
    return;
  }

  // fname = file_name(previous_object());

  if (minutes < 0) 
  {
    write("Argumento incorrecto.\n");
    return;
  }

  set_long("Es un Ángel femenino, de belleza inconmensurable, firmemente concentrada " +
           "en la cuenta atrás.\n");

  if (time_of_crash > 0)
    write("Ya estaba programado el reinicio del mud, dentro de "+
      (time_of_crash - time()) + " segundos.\n");

  time_of_crash = time() + minutes*60;

  write("Reinicio del mundo dentro de "+
    (time_of_crash - time()) + " segundos.\n");

  shutdown_in_progress = 1;
  set_heart_beat(1);
} /* shut() */
 
string long(string str, int dark) 
{
  if (time_of_crash && this_player()->query_coder())
   return ::long(str,dark) +
      "El destino del mundo se decidirá en "+(time_of_crash - time())+
      " segundos.\n";

  return ::long(str,dark);
}
 
void end_it_all() 
{
  int i;
  object *obs;
 
  ishout("¡Mud cerrándose!\n", 0);

  obs = users();
  
  for (i = 0; i < sizeof(obs); i++)
    call_out("force_quit", 0, obs[i]);
  
  closed = 1;
  call_out("blue", 5);
}
 
void force_quit(object ob) 
{
  if (ob && ob->player())
    ob->player()->really_quit(); // changed to really_quit - Radix
}
 
void blue() 
{
  // FLAG_GAME->set_flag(0);
  // FLAG_GAME->save_me();

  // neverbot, 7/03
  handler(WEATHER_HANDLER)->save_weather();

  shutdown(0);
}
 
int query_time_to_crash() 
{ 
  if (closed) 
    return 1;
  return time_of_crash - time(); 
}
 
int clean_up()
{
  return 1;
}
 
void dest_me() 
{
  if (!this_player())
  {
    log_file("game_log", "["+ctime(time())+"] Shutdown cancelled\n");
  }

  closed = 0;
  
  // Radix...
  if (this_player())
  {
    log_file("game_log", "["+ctime(time())+"] Shutdown cancelled by "+this_player()->query_cap_name()+".\n");
    catch(find_object("/obj/handlers/timed")->reset_shutting());
  }
  ::dest_me();
}
