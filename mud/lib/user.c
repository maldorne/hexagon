
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

inherit history     "/lib/user/history";
inherit alias       "/lib/user/alias";
inherit nickname    "/lib/user/nickname";
inherit prompt      "/lib/user/prompt";
inherit more_string "/lib/user/more_string";

inherit living      "/lib/living/living";
inherit role        "/lib/user/role";
inherit account     "/lib/user/account";

inherit events      "/lib/core/basic/events";
inherit auto_load   "/lib/core/basic/auto_load";
inherit output      "/lib/user/output";
inherit migration   "/lib/user/migration";

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


// TMP DEBUG, REMOVE!!!
// string query_name() { return "neverbot"; }
// string short(varargs int dark) { return "neverbot"; }
int query_cols() { return 79; }
int query_rows() { return 20; }
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
  output::create();
  migration::create();

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

  living::init();
}

int query_player() { return 1; }
int query_user() { return 1; }
// total time connected
int query_time_on() { return time_on - time(); }
int query_last_log_on() { return last_log_on; }
nomask void set_ontime(int i) { ontime = i; }
nomask int query_ontime() { return ontime; }

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
    write("¡Vuelve dentro de poco!\n");  
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
  string oldeuid;
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
  
  oldeuid = geteuid();

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
      tmp_redirect_obj =  redirect_input_ob;
      tmp_redirect_func = redirect_input_function;
      tmp_redirect_args = redirect_input_args;
    
      redirect_input_ob = nil;
      redirect_input_function = "";
      redirect_input_args = ({ });

      call_other(tmp_redirect_obj, tmp_redirect_func, str, tmp_redirect_args...);
  
      if (!redirect_input_ob && 
          this_object() && this_object()->query_show_prompt())
      {
        show_prompt();
      }
      
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
      str = expand_history(str);
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
  // do not forget!!
  ::heart_beat();

  // write("hb: " + object_name(this_object()) + "\n");
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

int really_quit()
{
  object *ob, money;
  object frog, frog2;
  int i;
  int secure;
  string aux;

  aux = "";
  secure = 0;

  // quit_destination added by neverbot, 03/09  
  while (environment(this_object()) &&
        (aux = environment(this_object())->query_quit_destination()) && 
        (secure < 10))
  {
    if (load_object(aux))
      this_object()->move(aux);
    secure++;
  }
  
  if (secure != 0) 
  {
    tell_object(this_object(), "En tu localización previa no es posible salir, "+
        "has sido movido a la más cercana (vuelve a intentarlo aquí si aún "+
        "deseas desconectar).\n");
    this_object()->do_look();
    this_object()->save_me();
    return 1;
  }

  traverse_timed_properties();
  last_log_on = time();
  
  this_object()->adjust_online_time(time() - ontime);

  // TODO timekeeper
  /* This should send the time the player was on to an object which keeps
   * track of total hours played (non-immortal) Anirudh - March 17, 1996 */
  // if (!this_object()->query_coder())
  //   TIMEKEEPER->add_time(time() - ontime);

  if (this_object()->query_name() != DEF_NAME)
  {
    if (this_object()->query_property(GUEST_PROP))
      log_file("enter", sprintf("Exit  : %-15s %s (guest) [%s]\n",
                                name, ctime(time(),4),
                                query_ip_number(this_object())+" ("+query_ip_name(this_object())+")"));
    else
      log_file("enter", sprintf("Exit  : %-15s %s [%s]\n",
                                name, ctime(time(),4), 
                                query_ip_number(this_object())+" ("+query_ip_name(this_object())+")"));
  }

  // TODO editor
  // catch(editor_check_do_quit());

  // TODO social
  // if (query_race_ob())
  //   catch(query_race_ob()->player_quit(this_object()));
  // if (query_guild_ob())
  //   catch(query_guild_ob()->player_quit(this_object()));
  // if (query_class_ob())
  //   catch(query_class_ob()->player_quit(this_object()));
  // if (query_deity_ob())
  //   catch(query_deity_ob()->player_quit(this_object()));
  // if (query_group_ob())
  //   catch(query_group_ob()->player_quit(this_object()));
  // if (query_race_group_ob())
  //   catch(query_race_group_ob()->player_quit(this_object()));
  // if (query_job_ob())
  //   catch(query_job_ob()->player_quit(this_object()));
  // if (query_city_ob())
  //   catch(query_city_ob()->player_quit(this_object()));

  // curses_quit();

  // Antes del save_me, movemos todos los objetos que no vamos a grabar a
  // la room del jugador
  if (environment(this_object()))
  {
    ob = deep_inventory(this_object());
    for(i = 0; i < sizeof(ob); i++)
    {
      if (ob[i]->query_no_save_object())
      {
        ob[i]->move(environment(this_object()));
        tell_object(this_object(), "Dejas tu "+ob[i]->short()+".\n");
      }
    }
  }

  // write("Thanks for playing see you next time.\n");
  tell_player(this_object(), "Gracias por jugar, ¡hasta la próxima!\n");

  if (query_name() != DEF_NAME)
  {
    if ( !this_object()->query_hidden() )
      tell_room( environment(this_object()), this_object()->query_cap_name() + 
        " sale de "+mud_name()+".\n");

    if ( this_object()->query_coder() )
      event(users(), "inform", this_object()->query_cap_name() +
                               " sale de " + mud_name(), "logon-coders",
                               all_inventory(environment(this_object())));
    else
      event(users(), "inform", this_object()->query_cap_name() + 
                               " sale de " + mud_name(), "logon",
                               all_inventory(environment(this_object())));
  }

  ob = ({ });
  i = 0;

  save_me();
  
  // TODO mounts
  // mounts, neverbot 07/05
  // save_mount();
  // if (query_mount())
  //   query_mount()->owner_quit();

  /* get rid of the money.... we dont want them taking it twice now do we? */
  if ((money = present(MONEY_NAME, this_object())))
      money->dest_me();

  frog = first_inventory(this_object());
  while (frog) 
  {
    frog2 = next_inventory(frog);

    // if (frog)

    // hack para los objetos unicos... si los destruimos con la funcion
    // dest_me el handler no los almacenara como grabados en la ficha
    // de un player, sino como destruidos.
    if (frog->query_unique_object())
    {
      tell_object(find_living("folken"), "Dest unique del objeto "+frog->short()+".\n");
      frog->dest_unique();
    }
    else if (frog->query_auto_load() ||
             frog->query_static_auto_load())
    {
      frog->dest_me();
    }

    frog = frog2;
  }

  transfer_all_to(environment());
  ob = all_inventory(this_object());
  
  for (i = 0; i < sizeof(ob); i++)
    ob[i]->dest_me();

  // TODO safety here?
  find_object(USER_HANDLER)->remove_user(this_object());
  find_object(LIVING_HANDLER)->remove_living(this_object());

  dest_me();

  return 1;
} /* really quit() */

void continue_quit(int a, object here)
{
  if (this_object()->query_is_fighting()) 
  {
    tell_object(this_object(), "Debes terminar primero tus combates.\n "+
        "Escribe '%^BOLD%^detener combates%^RESET%^' para terminarlos lo antes posible.\n");
    return;
  }

  if (environment(this_object()) != here) 
  {
    tell_object(this_object(),"Si no paras de moverte será imposible buscar un buen lugar "+
        "para salir del juego.\n");
    return;
  }

  if (a <= 0)
  {
    this_object()->really_quit();
    return;
  }

  call_out("continue_quit", 0, a-1, environment(this_object()));
}

int quit()
{
  if (query_loading() || query_property(LOADING_PROP))
  {
    notify_fail("Todavía se esta cargando tu equipo, espera a que termine el proceso.\n");
    return 0;
  }

  tell_object(this_object(), "Saliendo...\n");
  
  if (this_object()->query_coder()) 
  {
    this_object()->really_quit();
    return 1;
  }
  
  tell_room(environment(this_object()), this_object()->query_cap_name()+
      " busca un lugar cómodo para salir del juego.\n", ({this_object()}));

  if (this_object()->query_is_fighting())
  {
    tell_object(this_object(),"Debes terminar primero tus combates.\n"+
        "Escribe '%^BOLD%^detener combates%^RESET%^' para terminarlos lo antes posible.\n");
    save_me();
  }
  else
    call_out("continue_quit", 0, 5, environment(this_object()));
  
  return 1;
} 

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
               output::stats();
}

