
#include <kernel.h>
#include <basic/id.h>
#include <user/user.h>
#include <user/roles.h>
#include <user/player.h>
#include <user/input.h>
#include <mud/secure.h>
#include <living/living.h>
#include <common/properties.h>
#include <basic/money.h>
#include <user/hud.h>
#include <language.h>

inherit history     "/lib/user/history";
inherit alias       "/lib/user/alias";
inherit nickname    "/lib/user/nickname";
inherit prompt      "/lib/user/prompt";
inherit more_string "/lib/user/more_string";
inherit more_file   "/lib/user/more_file";

inherit living      "/lib/living/living";
inherit role        "/lib/user/role";
inherit account     "/lib/user/account";

inherit events      "/lib/core/basic/events";
inherit auto_load   "/lib/core/basic/auto_load";
inherit output      "/lib/user/output";
inherit migration   "/lib/user/migration";
inherit past        "/lib/user/past";
inherit help        "/lib/user/help";
inherit quests      "/lib/user/quests";
inherit weather     "/lib/user/weather";
inherit read        "/lib/user/read";

static object redirect_input_ob;       // object that will catch input and
static string redirect_input_function; // function inside that object
static mixed  redirect_input_args;     // optional arguments passed to the function

// last input time
static int timestamp;

string last_pos;     // file name of environment of last connection
string *auto_load;   // inventory
int time_on;         // total time connected 
int ontime;          // session time
int last_log_on;     // time of last log on
string last_on_from; // last ip the user connected from
string hud;

static int save_counter; // each reset counter
static int last_command; // time of last command
static int net_dead;     // has lost connection?

// TMP DEBUG, REMOVE!!!
// string query_name() { return "neverbot"; }
// string short(varargs int dark) { return "neverbot"; }
int query_cols() { return 79; }
int query_rows() { return 100; }
void set_cols(int i) { }
int query_verbose() { return 1; }

// stats
void load_mount();


int check_dark(int light)
{
  if (this_object()->query_dead())
    return 0;
  if (this_object()->query_race_ob())
    return (int)query_race_ob()->query_dark(light);
  return 0;
}
// END TMP DEBUG





#include "/lib/user/start.c"
#include "/lib/user/death.c"
#include "/lib/user/quit.c"

// Function prototypes
// static void open();
// static void close(varargs mixed dest);
// static void receive_message(string str);
// nomask int set_input_to(object obj, string func, varargs int flag, mixed arg) 

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
  prompt::create();
  role::create();
  account::create();
  more_string::create();
  more_file::create();
  output::create();
  migration::create();
  past::create();
  help::create();
  quests::create();
  weather::create();
  read::create();

  // the last one
  living::create();

  seteuid(PLAYER_EUID);

  // already done in living
  // enable_commands();

  timestamp    = 0;
  time_on      = time();
  ontime       = time();
  last_log_on  = time();
  auto_load    = ({ });
  last_on_from = "";
  last_pos     = "";
  
  hud          = HUD_DIFFICULTY; 

  save_counter = 0;
  last_command = time();
  net_dead     = 0;

  redirect_input_ob       = nil;
  redirect_input_function = "";
  redirect_input_args     = ({ });

  if (clonep(this_object()))
    set_heart_beat(1);
}

void init()
{
  history::init();
  alias::init();
  nickname::init();
  weather::init();

  living::init();
}

int query_player() { return 1; }
int query_user() { return 1; }
// total time connected
int query_time_on() { return time_on - time(); }
int query_last_log_on() { return last_log_on; }
nomask void set_ontime(int i) { ontime = i; }
nomask int query_ontime() { return ontime; }
nomask int query_timestamp() { return timestamp; }

void dest_me()
{
  role::dest_me();
  account::dest_me();

  // remove the user from the user handler
  if (clonep(this_object()))
    find_object(USER_HANDLER)->remove_user(this_object());

  // main dest_me
  living::dest_me();
}

nomask int save_me();

void reset()
{
  if(save_counter > 0)    
  {
    update_tmps();
    save_me();
  }
  else
    ++save_counter;
}

// Called from the input_to efun
nomask int set_input_to(object obj, string func, varargs int flag, mixed args...) 
{
  if (redirect_input_ob == nil) 
  {
    redirect_input_ob = obj;
    redirect_input_function = func;
    redirect_input_args = args;

    // if (flag)
    //   send_message(echo = 0);
  
    return TRUE;
  }

  return FALSE;
}

// called from the driver
static void open()
{
  timestamp = time();

  LOGIN->logon(this_object());
}

static nomask void disconnect(varargs int silence)
{
  if (!silence)
    write(_LANG_COME_AGAIN_SOON);
}

// called from the driver
static void close(mixed arg)
{
  disconnect(FALSE);
}

nomask int restore_me()
{
  if (!SECURE->valid_progname("/lib/core/login"))
    return 0;

  return restore_object("/save/players/" + 
                        this_object()->query_name()[0..0] + "/" +
                        this_object()->query_name() + ".o", 1);
}

nomask int save_me()
{
  // string oldeuid;
  object ob;
  // if (!SECURE->valid_progname("/lib/core/login"))
  //   return 0;

  if (query_loading() || query_property(LOADING_PROP))
    return 0;

  if (query_property(GUEST_PROP)) 
  {
    tell_object(this_object(), "Ups, l"+G_CHAR+"s invitad"+G_CHAR+"s no pueden salvar...\n");
    return 0;
  }

  // Fix by Wonderflug.  Saving object is a bad idea.
  if (!strlen(query_name()) || (query_name() == DEF_NAME))
    return 0;

  if ((this_object()->query_role_name() == PLAYER_ROLE) ||
      (member_array(query_verb(), ({ "save", "salvar", "grabar" })) != -1))
  {
    tell_object(this_object(), "Salvando...\n");
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
    last_pos = file_name(environment());
  else
    last_pos = START_POS;

  auto_load = create_auto_load(all_inventory());
  time_on -= time();

  // seteuid(ROOT);
  catch(save_object("/save/players/" + 
                    this_object()->query_name()[0..0] + "/" +
                    this_object()->query_name() + ".o", 1));  

  // seteuid(oldeuid);
  time_on += time();

  return 1;
}

void send_message(string str)
{
  if (str == nil)
    return;

  ::send_message(str);
}

// called from the driver

// the skeleton of this function started taking the melville received_message 
// as the general idea, but I think it will change a lot with time
static void receive_message(string str) 
{
  string tmp_redirect_func;
  object tmp_redirect_obj;
  mixed  tmp_redirect_args;
  string verb;
  string params;
  string * pieces;
  int i;

  rlimits (MAX_USER_DEPTH; MAX_USER_TICKS) 
  {
    timestamp = time();

    // input_to redirection
    if (redirect_input_ob) 
    {
      // to have this_player() inside an input_to redirection
      MUDOS->set_initiator_player(this_object());
      MUDOS->set_initiator_object(this_object());

      tmp_redirect_obj  = redirect_input_ob;
      tmp_redirect_func = redirect_input_function;
      tmp_redirect_args = redirect_input_args;
    
      redirect_input_ob       = nil;
      redirect_input_function = "";
      redirect_input_args     = ({ });

      call_other(tmp_redirect_obj, tmp_redirect_func, str, tmp_redirect_args...);
  
      if (!redirect_input_ob && 
          this_object() && this_object()->query_show_prompt())
      {
        show_prompt();
      }

      // the object destructed itself
      if (!this_object()) {
        return;
      }

      MUDOS->set_initiator_player(nil);
      MUDOS->set_initiator_object(nil);

      return;
    }

    // old ccmudlib process_input content

    // Taniwha crash workround
    // if ( !strsrch(str,"%^") ) 
    //   return;

    // while ( str[<1..<1] == " " )
    //    str = str[0..<2];
    str = trim(str);

    if ( !strlen(str) || str == "\n" ) 
    {
      // \n added before the prompt 
      show_prompt("\n");
      return;
    }

    // the new line has content, so we have a new this_player()
    MUDOS->set_initiator_player(this_object());
    MUDOS->set_initiator_object(this_object());

    if ( strlen(str) > INPUT_MAX_STRLEN ) 
    {
      str = str[ 0..INPUT_MAX_STRLEN ];
      write("Comando demasiado largo - procesando de todas formas.\n");
    }

    // Ok, my stuff: (Baldrick)

    // Might as well move this here too
    if (str[0] == '.')
    {
      this_object()->set_trivial_action();
      str = expand_history(str[1..]);
    }
    else
    {
      // Bishop - adding history
      add_history( str );      
    }

    sscanf(str, "%s %s", verb, params);

    if (!verb)
      verb = str;

    // First the aliases
    if ( !exec_alias(verb, params) )
    {
      // if no alias found, continue
      action_check( str );
      lower_check( str );      
    }

    // the object destructed itself
    if (!this_object())
      return;

    MUDOS->set_initiator_player(nil);
    MUDOS->set_initiator_object(nil);

  } // rlimits
}

void heart_beat()
{
  if (this_object() && !interactive(this_object()) )
  {
    if (name == "guest" || name == "root")
    {
      say(query_cap_name()+" es engullid"+G_CHAR+" por una nube de lógica.\n");
      quit();
    }
    else if (!net_dead)
    {
      say(query_cap_name()+" se vuelve blanc"+G_CHAR+" y sólid"+G_CHAR+" al tiempo que se "+
        "convierte en una estatua.\n");
      event(users(), "inform", query_cap_name() + " ha perdido " +
        query_possessive() + " conexión", "conexiones");
      save_me();
      quit();
      net_dead = 1;
    }
    // last_command = time() - this_object()->query_idle();
  }

  // query_idle no se puede ejecutar sobre objetos no interactive,
  // asi que saco esta linea del if anterior, neverbot 7/05
  if (!net_dead)
    last_command = time() - this_object()->query_idle();

  living::heart_beat();
}

void run_away()
{
  mixed *direcs;
  int i, bong;

  if ( this_object()->query_property(PASSED_OUT_PROP) )
  {
    tell_object(this_object(),"No estás en condiciones de salir corriendo.\n");
    return ;
  }

  direcs = (mixed *)environment()->query_dest_dir();
  
  while (!bong && sizeof(direcs))
  {
    i = random(sizeof(direcs)/2)*2;
    bong = this_object()->insert_action(direcs[i]);
    if (!bong)
      direcs = delete(direcs, i, 2);
    else
      write("Ves a tus pies correr ante ti.\n");
  }
  if (!bong)
    write("Intentaste escapar, pero no has encontrado "+
      "ninguna salida.\n");
}

/* old glance(), neverbot 21/4/2003 */
int do_glance(varargs string arg)
{
  // already in the cmd
  // if (this_object()->query_coder())
  //   if (!strlen(arg) && environment())
  //     write(object_name(environment()) + "\n");

  // Externalized - Radix
  if (!arg || !strlen(arg)) 
      return do_command("glance");
  return do_command("glance " +arg);
}

/* old look_me(), neverbot 21/4/2003 */
int do_look(varargs string arg)
{
  if (!this_object()->query_verbose())
    return do_glance(arg);

  // already in the cmd
  // if (this_object()->query_coder())
  //   if ((!arg || !strlen(arg)) && environment())
  //     write(object_name(environment()) + "\n");

  if (!arg || !strlen(arg)) 
    return do_command("look");
  return do_command("look " + arg);
}

// query_idle - determine how many seconds an interactive player has been idle
// int query_idle( object ob );
// Query how many seconds a player object (ob) has been idling.

nomask int query_idle()
{
  return time() - this_object()->query_timestamp();
}

// Funciones para el apuntador (forma de percibir a otros personajes)
string query_hud() { return hud; }
void set_hud(string type) { if (member_array(type, HUD_TYPES) != -1) hud = type; }

mixed * stats() 
{
  return ({ 

    ({ "Timestamp", timestamp, }),
    ({ "Last Pos", last_pos }),
    ({ "Auto Load", auto_load }),
    ({ "Time On", time_on }),
    ({ "On Time", ontime }),
    ({ "Last Logon", last_log_on }),
    ({ "Last On From", last_on_from }),

          }) + history::stats() + 
               alias::stats() + 
               nickname::stats() + 
               prompt::stats() + 
               living::stats() +
               role::stats() +
               account::stats() + 
               migration::stats() + 
               past::stats() + 
               help::stats() + 
               quests::stats() + 
               weather::stats() + 
               read::stats() + 
               output::stats();
}
