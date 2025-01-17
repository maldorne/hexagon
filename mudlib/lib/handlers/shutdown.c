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
#include <language.h>

inherit "/lib/core/object";

#define IMUD_SERV "/net/intermud3/services"
// #define FLAG_GAME "/d/flagomiq/handler/hand_flag"
 
// #define REBOOT_TIME 23 * 60 * 60 * 2  // every 23 hours * 2, so its not always
//                                       // the same time 
#define REBOOT_MEMORY 42 * 1024 * 1024

int shutdown_in_progress;
int time_of_crash;
int closed;
int ishout(string str, varargs int muff, int notification);
void do_delay_prop();

void create()
{
  ::create();
  time_of_crash = 0;
  shutdown_in_progress = 0;
}
 
void setup() 
{
  // This stops wizzes from cloning armageddon. He only needs to be loaded
  if (file_name(this_object()) != "/lib/handlers/shutdown") 
  {
    write("This object cannot be cloned.\n");
    dest_me();
    return;
  }

  set_name(_LANG_SHUTDOWN_NAME);
  set_living_name(_LANG_SHUTDOWN_NAME);
  set_short(_LANG_SHUTDOWN_SHORT);
  set_long(_LANG_SHUTDOWN_LONG);
  add_alias("shutdown");

  move("bing");

  reset_get();
  reset_drop();

  call_out("do_delay_prop", 0);

  // Radix...
  // if (this_player())
  // {
     // call_out("delayed_safety", 5, this_player()->query_cap_name());
     // tell_room(environment(this_object()),"The Herald starts a countdown of 5 seconds!\n");
  // }
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
/*
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
*/
 
// Radix was here
int ishout(string str, varargs int muff, int notification)
{
  object * all;
  int i;

  all = users();

  for (i = 0; i < sizeof(all); i++)
  {
    if (all[i])
    {
      if (muff && all[i]->query_earmuffs()) 
        continue;

      if (notification)
        all[i]->add_notification("mud", str);
      else
        tell_object(all[i], "\n" + str + "\n");
    }
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
    ishout(_LANG_SHUTDOWN_IN_SECONDS, 0);
    return;
  }

  if (time_to_crash < 60 && time_to_crash % 10 < 2) 
  {
    ishout(_LANG_SHUTDOWN_IN_SECONDS, 0);
    return;
  }

  if (time_to_crash % 60 > 1) 
    return;
  
  time_to_crash /= 60;
  
  if (time_to_crash < 10 || !(time_to_crash % 10)) 
  {
    if (time_to_crash == 1)
      ishout(_LANG_SHUTDOWN_IN_ONE_MINUTE, 0, 1);
    else
      ishout(_LANG_SHUTDOWN_IN_MINUTES, 1, 1);

    return;
  }
} /* heart_beat() */
 
void shut(int minutes)
{
  if (!intp(minutes)) 
  {
    write(_LANG_SHUTDOWN_WRONG_PARAM);
    return;
  }

  if (minutes < 0) 
  {
    write(_LANG_SHUTDOWN_WRONG_PARAM);
    return;
  }

  set_long(_LANG_SHUTDOWN_LONG2);

  if (time_of_crash > 0)
    write(_LANG_SHUTDOWN_ALREADY);

  time_of_crash = time() + minutes * 60;

  write(_LANG_SHUTDOWN_ACCEPT);

  shutdown_in_progress = 1;
  set_heart_beat(1);
} /* shut() */
 
string long(string str, int dark) 
{
  if (time_of_crash && this_player()->query_coder())
   return ::long(str,dark) + _LANG_SHUTDOWN_EXTRA_LONG;

  return ::long(str,dark);
}
 
void end_it_all() 
{
  int i;
  object *obs;
 
  ishout(_LANG_SHUTDOWN_MUD_CLOSING, 0);

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
  closed = 0;
  
  // Radix...
  if (this_player())
    log_file("game_log", "[" + ctime(time()) + "] Shutdown cancelled by " + 
             this_player()->query_cap_name() + ".\n");
  else
    log_file("game_log", "[" + ctime(time()) + "] Shutdown cancelled\n");

  ::dest_me();
}

/*
 * This function will automatically reboot the mud every REBOOT_TIME seconds
 * Only calls a shutdown if we have been up long enough, and no shutdown
 * is already running.
 *
 * Called from /lib/handlers/cron.c
 */
void auto_reboot() 
{
  if (!shutdown_in_progress) 
  {
    shut(10);

    shout(_LANG_SHUTDOWN_AUTO_REBOOT);
    log_file("game_log", "[" + ctime(time()) + "] Auto reboot started " +
        "(uptime: " + (uptime()/3600) + " hours - use of memory: " +
        memory_info() + ")\n");
  }
}

/*
 * This function will automatically reboot the mud every if used memory 
 * is higher than REBOOT_MEMORY.
 *
 * Called from /lib/handlers/cron.c
 */
void memery_reboot()
{
  if (memory_info() > REBOOT_MEMORY)
  {
    auto_reboot();
  }
}
