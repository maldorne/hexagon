
#include <kernel.h>
#include <user/user.h>
#include <user/input.h>

inherit login     "/lib/user/login";
inherit history   "/lib/user/history";
inherit alias     "/lib/user/alias";
inherit nickname  "/lib/user/nickname";
inherit prompt    "/lib/user/prompt";

inherit living    "/lib/living/living";
inherit role      "/lib/user/role";

inherit events    "/lib/core/basic/events";
inherit auto_load "/lib/core/basic/auto_load";

// tmp for testing commands
inherit tmp       "/lib/coder/tmp";

static object redirect_input_ob;       // object that will catch input and
static string redirect_input_function; // function inside that object
static mixed  redirect_input_args;     // optional arguments passed to the function

// Function prototypes
// static void open();
// static void close(varargs mixed dest);
// static void receive_message(string str);
// nomask int set_input_to(object obj, string func, varargs int flag, mixed arg) 

void create() 
{
  login::create();
  history::create();
  alias::create();
  nickname::create();
  prompt::create();
  living::create();
  role::create();

  // already done in living
  // enable_commands();

  // this should be done both for players and npcs
  set_living_name("neverbot");

  redirect_input_ob       = nil;
  redirect_input_function = "";
  redirect_input_args     = ({ });

  if (clonep(this_object()))
    set_heart_beat(1);

  // if (object_name(previous_object())!=DRIVER) {
  //   destruct_object(this_object());
  // }
}

void init()
{
  history::init();
  alias::init();
  nickname::init();

  living::init();
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
  login::logon();
}

// called from the driver
static void close(mixed arg)
{
  login::disconnect(FALSE);
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
  
      if (!redirect_input_ob && this_object())
        show_prompt();
      
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

    if( strlen(str) > INPUT_MAX_STRLEN ) 
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

    MUDOS->set_initiator_player(nil);
    MUDOS->set_initiator_object(nil);

  } // rlimits
}


void heart_beat()
{
  // do not forget!!
  ::heart_beat();

  // write("hb: " + object_name(this_object()) + "\n");
  // send_message("hb: " + object_name(this_object()) + "\n");
}

int query_player() { return 1; }
int query_user() { return 1; }

// static int echo;     is input echoing turned on 
// static int editing;   /* are we editing? */

mixed * stats() 
{
  return ({ }) + login::stats() + 
                 history::stats() + 
                 alias::stats() + 
                 nickname::stats() + 
                 prompt::stats() + 
                 living::stats() +
                 role::stats();
}










// TMP DEBUG, REMOVE!!!
int query_coder() { return 1; }
string query_name() { return "neverbot"; }
string short(varargs int dark) { return "neverbot"; }
int query_cols() { return 79; }
int query_invis() { return 0; }

object query_race_ob() { return nil; }
int query_volume(int i) { return 0; }

int check_dark(int light)
{
  if (this_object()->query_dead())
    return 0;
  if (this_object()->query_race_ob())
    return (int)query_race_ob()->query_dark(light);
  return 0;
}

int do_look() 
{
  return do_cmd("mirar");
}


/* Control over editing function. Can come only from the associated
   inner_player object */

// void set_in_edit (int foo) {
//     if (previous_object()!=inner_player) return;
//     editing = foo;
// }

// int query_in_edit() {
//     return editing;
// }

// int query_in_input() {
//     if (redirect_input_ob) return 1;
//     return 0;
// }

/* The destruct() function overrides the one defined in the auto object,
   making it static. User objects can only call dest internally: if you
   need to dest one, call quit() in it (though that requires permissions
   as well.)
   This protection does not apply to the master copy of the object,
   since you need to remove that to update.
*/

// int destruct() {
//     if (clone_num(this_object())) {
//         if (previous_object()!=inner_player) return 0;
//     }
//     CHANNEL_D->remove_user_from_all(this_object());
//     destruct_object(this_object());
//     return (this_object()==0);
//   }

/* The quit() function is called from the inner_player object when the
   inner_player types the quit command. Save the user and destruct. */

// void quit() {

//     string str;

//     if (inner_player && previous_object()!=inner_player) return;
//     last_on = time();
//     str = query_ip_number(this_object());
//     if (str) last_ip = str;
//     save_object(USER_SAVE_DIR+name+USER_SAVE_EXT);
//     USERS_D->remove_user(this_object(),name);
//     log_file(USAGE_LOG,cap_name+" quit from "+query_ip_number(this_object())+
//        " at "+ctime(time())+"\n");
//     destruct();
// }

/* Write the edit-time prompt. */

// void write_edit_prompt() {
//     if (query_editor(inner_player)=="insert") {
//         send_message("*\b");
//     } else {
//         send_message(": ");
//     }
// }

/*
 * NAME:  catch_tell()
 * DESCRIPTION: catch a message
 */
// void catch_tell(string str) {

//     if (str == nil) {
//   return;
//     }
//     send_message(str);
// }

/*
 * NAME:  query_idle()
 * DESCRIPTION: return idle time of this user
 */
// int query_idle() {
//     return time() - timestamp;
// }

/* The linkdead function checks the user's IP. If there is none, he's
   linkdead and we return 1. If there is one, we return 0. */

// int linkdead() {
//     if (query_ip_number(this_object())) return 0;
//     return 1;
// }
