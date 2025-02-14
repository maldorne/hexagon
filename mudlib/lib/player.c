
#include <kernel.h>
#include <basic/id.h>
#include <user/player.h>
#include <user/roles.h>
#include <mud/secure.h>
#include <mud/config.h>
#include <living/living.h>
#include <areas/weather.h>
#include <common/properties.h>
#include <basic/money.h>
#include <language.h>
#include <translations/cmds.h>

inherit living      "/lib/living/living.c";

inherit auto_load   "/lib/core/basic/auto_load.c";

inherit events      "/lib/player/events.c";
inherit history     "/lib/player/history.c";
inherit alias       "/lib/player/alias.c";
inherit nickname    "/lib/player/nickname.c";
inherit migration   "/lib/player/migration.c";
inherit past        "/lib/player/past.c";
inherit quests      "/lib/player/quests.c";
inherit weather     "/lib/player/weather.c";
inherit read        "/lib/player/read.c";
inherit health      "/lib/player/health.c";

static object _user;      // the user/account object that handles the connection
string account_name;      // user email, used to find the owner account
string role_name;         // role this player has (player, coder, etc)

string last_pos;          // file name of environment of last connection
mapping auto_load;        // inventory
int time_on;              // total time connected
int ontime;               // session time
int last_log_on;          // time of last log on

static int save_counter;  // each reset counter
static int last_command;  // time of last command
static int _net_dead;     // has lost connection?

// Function prototypes
nomask object user();
nomask int query_coder();
nomask int save_me();

#include "/lib/player/start.c"
#include "/lib/player/death.c"
#include "/lib/player/quit.c"
#include "/lib/player/dark.c"

void create()
{
  // avoid calling create() on already cloned players
  if (name)
  {
    // event(admins(), "inform", this_player()->query_name() +
    //   " called create() on " + name, "cheat");
    return;
  }

  history::create();
  alias::create();
  nickname::create();
  migration::create();
  past::create();
  quests::create();
  weather::create();
  read::create();
  health::create();

  // must be the last one
  living::create();

  seteuid(PLAYER_EUID);

  account_name = "";
  // player role by default
  role_name    = PLAYER_ROLE;

  time_on      = time();
  ontime       = time();
  last_log_on  = time();
  auto_load    = ([ ]);
  last_pos     = "";
  _net_dead    = 0;

  save_counter = 0;
  last_command = time();

  if (clonep(this_object()))
    set_heart_beat(1);

  // TODO
  alias_commands();
  nickname_commands();
  // recipes_commands();
}

void init()
{
  history::init();
  alias::init();
  nickname::init();
  weather::init();

  // main inherit last
  living::init();
}

int move(mixed dest, varargs mixed messin, mixed messout)
{
  int i;

  if (environment() && (environment()->query_outside()))
    handler(WEATHER_HANDLER)->unnotify_me(environment());

  i = ::move(dest, messin, messout);

  if (environment() && (environment()->query_outside()))
    handler(WEATHER_HANDLER)->notify_me(environment());

  return i;
}

nomask int query_link() { return 0; }
nomask int query_player() { return 1; }
nomask int query_user() { return 0; }
nomask object user() { return _user; }
nomask int query_coder()
{
  if (_user)
    return _user->query_coder();
  return 0;
}
nomask int query_admin()
{
  if (_user)
    return _user->query_admin();
  return 0;
}
nomask int query_invis()
{
  if (_user)
    return _user->query_invis();
  return 0;
}
nomask int set_invis(int i)
{
  if (_user)
    return _user->set_invis(i);
  return 0;
}

nomask void set_user_ob(object ob)
{
  // for safety reasons, we allow this call only from /lib/core/login
  if (!SECURE->valid_progname("/lib/core/login"))
    return;

  _user = ob;
}

void set_name(string str)
{
  // default plural "persons that look like x"
  set_main_plural(capitalize(_LANG_PLAYER_PLURAL));
  add_plural(_LANG_PLAYER_PLURAL);
  ::set_name(str);
}

string query_account_name() { return account_name; }
void set_account_name(string str)
{
  // for safety reasons, we allow set_account_name only to be called from /lib/core/login
  if (!SECURE->valid_progname("/lib/core/login"))
    return;

  if (account_name != "")
    return; // changing account name is not allowed

  account_name = str;
}

string query_role_name() { return role_name; }

// total time connected
int query_time_on() { return time_on - time(); }
int query_last_log_on() { return last_log_on; }
nomask void set_ontime(int i) { ontime = i; }
nomask int query_ontime() { return ontime; }
nomask string query_last_pos() { return last_pos; }

void dest_me()
{
  // remove the user from the user handler
  // if (clonep(this_object()))
  //   find_object(USER_HANDLER)->remove_user(this_object());

  // main dest_me
  living::dest_me();
}

void reset()
{
  if (save_counter > 0)
  {
    update_tmps();
    save_me();
  }
  else
    ++save_counter;
}

nomask int restore_me()
{
  if (!SECURE->valid_progname("/lib/core/login"))
    return 0;

  return restore_object(PLAYERS_SAVE_DIR +
                        this_object()->query_name()[0..0] + "/" +
                        this_object()->query_name() + ".o", 1);
}

nomask int save_me()
{
  // string oldeuid;
  object ob;
  // if (!SECURE->valid_progname("/lib/core/login"))
  //   return 0;

  if (query_property(LOADING_PROP))
    return 0;

  if (query_property(GUEST_PROP))
  {
    tell_object(this_object(), _LANG_PLAYER_NO_GUEST_SAVE);
    return 0;
  }

  // Fix by Wonderflug.  Saving object is a bad idea.
  if (!strlen(query_name()) || (query_name() == DEF_NAME))
    return 0;

  if ((this_object()->query_role_name() == PLAYER_ROLE) ||
      (member_array(query_verb(), _LANG_PLAYER_SAVE_ACTIONS) != -1))
  {
    tell_object(this_object(), _LANG_PLAYER_SAVING);
  }

  if ((ob = present(MONEY_NAME, this_object())))
    money_array = (mixed *)ob->query_money_array();
  else
    money_array = ({ });

  // TODO social save
  // if ( query_guild_ob() && ( (file_size(query_guild_ob()) > 0) ||
  //   (file_size(query_guild_ob()+".c") > 0) ) )
    //   query_guild_ob()->player_save_me();
  // if (query_race_ob())
    //   query_race_ob()->player_save_me();

  // oldeuid = geteuid();

  if (environment())
  {
    if (environment()->query_location())
      last_pos = environment()->query_file_name();
    else
      last_pos = file_name(environment());
  }
  else
    last_pos = START_POS;

  auto_load = create_auto_load(all_inventory());
  time_on -= time();

  // seteuid(ROOT);
  catch(save_object(PLAYERS_SAVE_DIR +
                    this_object()->query_name()[0..0] + "/" +
                    this_object()->query_name() + ".o", 1));

  // seteuid(oldeuid);
  time_on += time();

  return 1;
}

void heart_beat()
{
  if (CONFIG_LOG_HEART_BEATS)
  {
    stderr("   + this_object()  : " + object_name(this_object()) + "\n");
    // stderr("   + this_player()  : " + (this_player() ? object_name(this_player()) : "nil") + "\n");
    // stderr("   + this_player(1) : " + (this_player(1) ? object_name(this_player(1)) : "nil") + "\n");
    // stderr("   + this_user()    : " + (this_user() ? object_name(this_user()) : "nil") + "\n");    
  }

  // if (this_player() && !interactive(this_player()) )
  // {
  //   if (name == "guest" || name == "root")
  //   {
  //     say(query_cap_name()+" es engullid"+G_CHAR+" por una nube de lógica.\n");
  //     quit();
  //   }
  //   else if (!_net_dead)
  //   {
  //     say(query_cap_name()+" se vuelve blanc"+G_CHAR+" y sólid"+G_CHAR+" al tiempo que se "+
  //       "convierte en una estatua.\n");
  //     event(users(), "inform", query_cap_name() + " ha perdido " +
  //       query_possessive() + " conexión", "conexiones");
  //     save_me();
  //     quit();
  //     _net_dead = 1;
  //   }
  //   // last_command = time() - user()->query_idle();
  // }

  // query_idle cannot be called on non interactive objects,
  // so this should be after the previous if, neverbot 7/05
  if (!_net_dead)
    last_command = time() - user()->query_idle();

  living::heart_beat();

  // TODO this is user stuff, move it to the user object when/if the user 
  // has its own heart_beat
  // show pending notifications after the heart beat has finished
  if (user()->query_pending_notifications())
    user()->show_notifications();
}

/* old glance(), neverbot 21/4/2003 */
int do_glance(varargs string arg) {
  // already in the cmd
  // if (this_object()->query_coder())
  //   if (!strlen(arg) && environment())
  //     write(object_name(environment()) + "\n");

  // Externalized - Radix
  if (!arg || !strlen(arg))
    return do_command(CMD_GLANCE_NAME);
  return do_command(CMD_GLANCE_NAME + " " + arg);
}

/* old look_me(), neverbot 21/4/2003 */
int do_look(varargs string arg) {
  if (!user()->query_verbose())
    return do_glance(arg);

  // already in the cmd
  // if (this_object()->query_coder())
  //   if ((!arg || !strlen(arg)) && environment())
  //     write(object_name(environment()) + "\n");

  if (!arg || !strlen(arg))
    return do_command(CMD_LOOK_NAME);
  return do_command(CMD_LOOK_NAME + " " + arg);
}

mixed * stats()
{
  return ({
    ({ "Account Name", account_name }),
    ({ "Role Name", role_name }),
    ({ "Last Pos", last_pos }),
    ({ "Auto Load", auto_load }),
    ({ "Time On", time_on }),
    ({ "On Time", ontime }),
    ({ "Last Logon", last_log_on }),
          }) + history::stats() +
               alias::stats() +
               nickname::stats() +
               living::stats() +
               migration::stats() +
               past::stats() +
               quests::stats() +
               weather::stats() +
               read::stats() +
               health::stats();
}
