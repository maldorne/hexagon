// This is the timeD, a time keeping daemon for LPC.
// This one does netdeads, idleouts and handles automatic reboots
// Written and Modified by Asmdoean, Skullslayer, Hamlet, Wonderflug, and
// many more :)

#include <mud/cron.h>
#include <mud/time.h>
#include <mud/config.h>
// #include <diplomacy.h>
#include <areas/weather.h>

inherit "/lib/core/object.c";

object * netdeads();
int query_player_object(object pob);

// ({ 
//    minutes, hours,
//    file, function,
//    last_minute_executed, last_hour_executed
// })
mixed * crontab;

// function names inside this object that will be called every cron_step
string * checks;

// Here are the function definitions
// This is so they are known to anything that wants to call them
void do_idletest();
void do_stat_users();
void net_dead_purge();

string * query_crontab() { return crontab; }

void create()
{  
  crontab = ({ });
  
  checks = ({
    // functions moved to the crontab
    // "do_idletest", 
    // "net_dead_purge",
    // "do_stat_users",
    // "update_guards",
  });  
  
  ::create();
}

void setup() 
{
  // anticloning
  if (file_name(this_object()) != "/lib/handlers/cron") {
    write("This object cannot be cloned.\n");
    dest_me();
    return;
  }
    
  set_name("cron handler");
  set_short("Cron handler daemon (cron.c)");
  set_long("This is the cron handler!\n");
  reset_get();

  add_property("no_clean_up", 1);

  call_out("read_crontab", 10); 

  call_out("cron_step", 15); 
  // call_out("cron_step_aux", 600);
  seteuid("root");
    
  // moved is masked, will always be moved to HOME
  move("bing");  
}

int move(mixed dest, varargs mixed messin, mixed messout) 
{
  ::move(HOME, messin, messout);
}

void read_crontab()
{
  string crontab_file_content;
  string * lines, * pieces;
  int i, j;

  crontab_file_content = read_file(CRONTAB);

  if (undefinedp(crontab_file_content))
    return;

  lines = explode(crontab_file_content, "\n");

  for (i = 0; i < sizeof(lines); i++)
  {
    string * elements;

    lines[i] = trim(lines[i]);

    // ignore comment lines
    if (!strlen(lines[i]) || lines[i][0..0] == "#")
      continue;

    pieces = explode(lines[i], " ");

    if (!sizeof(pieces) == 4)
    {
      debug("cron", "error with crontab, line: " + lines[i] + "\n");
      continue;
    }

    for (j = 0; j < sizeof(pieces); j++)
      pieces[j] = trim(pieces[j]);

    // TODO check time parameters, valid values are: 
    // *, */n, n
    // check the file loads
    if (!load_object(pieces[CRONTAB_OBJECT]))
    {
      debug("cron", "error with crontab, file does not load, line: " + lines[i] + "\n");
      continue;
    }
    // TODO: check function_exists in the object

    elements = ({ 
      pieces[CRONTAB_MINUTES],
      pieces[CRONTAB_HOURS],
      pieces[CRONTAB_OBJECT],
      pieces[CRONTAB_FUNCTION],
      -1, // last time executed (minutes)
      -1 // last time executed (hours)
     });

    crontab += ({ elements });
  }
}

void cron_step() 
{
  int i, size;

  if (CONFIG_LOG_CRONTAB)
    debug("cron", "cron_step " + file_name(this_object()) + "\n");

  size = sizeof(checks);

  for (i = 0; i < size; i++)
  {
    if (CONFIG_LOG_CRONTAB)
      debug("cron", "------> cron " + file_name(this_object()) + " " + checks[i] + "\n");

    catch(call_other(this_object(), checks[i]));
  }

  size = sizeof(crontab);

  for (i = 0; i < size; i++)
  {
    // ({ 
    //    minutes, hours,
    //    file, function,
    //    last_minute_executed, last_hour_executed
    // })
    string * pieces;
    int * times;
    int run_minutes, run_hours;

    run_minutes = false;
    run_hours = false;
    times = localtime(time());
    pieces = explode(crontab[i][CRONTAB_MINUTES], "/");

    // check minutes
    // */n
    if (sizeof(pieces) > 1)
    {
      if (times[LT_MIN] * atoi(pieces[1]) == 0)
        run_minutes = true;
    }
    // *
    else if (pieces[0] == "*")
      run_minutes = true;
    // n
    else if (times[LT_MIN] == atoi(pieces[0]))
      run_minutes = true;

    pieces = explode(crontab[i][CRONTAB_HOURS], "/");

    // check hours
    if (run_minutes)
    {
      // */n
      if (sizeof(pieces) > 1)
      {
        if (times[LT_HOUR] * atoi(pieces[1]) == 0)
          run_hours = true;
      }
      // *
      else if (pieces[0] == "*")
        run_hours = true;
      // n
      else if (times[LT_HOUR] == atoi(pieces[0]))
        run_hours = true;
    }

    if (run_minutes && run_hours)
    {
      if (CONFIG_LOG_CRONTAB)
        debug("cron", "------> crontab " + crontab[i][CRONTAB_OBJECT] + " " + crontab[i][CRONTAB_FUNCTION] + "\n");

      catch(call_other(load_object(crontab[i][CRONTAB_OBJECT]), crontab[i][CRONTAB_FUNCTION]));
      // update last execution time
      crontab[i][CRONTAB_LAST_MINUTE] = times[LT_MIN];
      crontab[i][CRONTAB_LAST_HOUR] = times[LT_HOUR];
    }
  }

  call_out("cron_step", 60);
}

// void dest_me() 
// {
//   remove_call_out(call_out_id);
//   ::dest_me();
// }

int clean_up()
{
  return 1;
}

// Extra safety, check the call_out still exists
// void cron_step_aux()
// {
//   if (find_call_out("cron_step") == -1)
//     call_out_id = call_out("cron_step", 60);     
//     
//   call_out("cron_step_aux", 600);
// }


// ********************************************************************************
//  from here every function should be moved to different handlers at some point
// ********************************************************************************


/*
* This is the function that will check every player to test for idleness
* by calling the idle_out() function in each player
*/
void do_idletest() 
{
  object * arr;
  int i, size;

  arr = users();
  size = sizeof(users());
  for (i = 0; i < size; i++) 
  {
    // check that the player didn't sneak out while we weren't looking
    if (userp(arr[i]))
      arr[i]->idle_out();
  }
}

void net_dead_purge() 
{
  // el do_force_on_me acaba llenando el log de forzar hasta limites insospechados
  // netdeads()->do_force_on_me("quit");
  // netdeads()->quit();
  // TODO: do this, get list of netdeads() and purge the user list
} 

// int query_player_object(object pob) 
// {
//   return (file_name(pob)[0..7] == "/global/" && clonep(pob));
// } 

object * netdeads()
{
  // TODO: do this 
  /*
  object *l, *nd;
  int i;

  l = ({ });
  nd = ({ });

  // livs = livings();
  // l = filter_array(livs, "query_player_object",this_object());

  l = users();

  for (i = 0; i < sizeof(l); i++)
      if (member_array(l[i], users()) == -1)
          nd += ({ l[i] });
  return nd;
  */
 return ({ });
}

void do_stat_users()
{
  object * foos;
  int i, mortals, immortals;

  mortals = 0, immortals = 0;

  foos = users();

  for (i = 0; i < sizeof(foos); i++)
  {
    if ((int)foos[i]->query_coder() == 1)
      immortals++;
    else
      mortals++;
  }

  if (immortals > 0 || mortals > 0)
    log_file("connected", "["+ctime(time(), 4)+"] " + immortals + " (coders), " + mortals + " (players)\n");

  return;
}

/*
void update_guards()
{
  object handler;

  // One more guard every GUARD_TIME seconds
  if ((uptime() < 120) || (time() > last_guard_time + GUARD_TIME))
  {
    // log_file(LOG_DIPLOMACY, "[" + ctime(time(), 4) + "] timed update_guards\n");

    last_guard_time = time();

    handler = load_object( DIPLOMACY_HANDLER );

    if (!handler)
      return;

    handler->update_guards();

    // From minute one (first call of update_guards) to minute two, lots of guards
    if (uptime() < 120)
      call_out("update_guards", 1);
  }
}
*/