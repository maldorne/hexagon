
#include <kernel.h>
#include <user/user.h>
#include <user/input.h>

inherit login   "/lib/user/login";
inherit history "/lib/user/history";
inherit alias   "/lib/user/alias";
inherit prompt  "/lib/user/prompt";

inherit living  "/lib/living/living";




// tmp, should be in the player object
inherit events  "/lib/core/basic/events";
// tmp for testing commands
inherit tmp     "/lib/coder/tmp";

// static string name;         // user name != inner_player name ?
// static object inner_player; // The inner_player object attached to this user

static object redirect_input_ob;       // object that will catch input and
static string redirect_input_function; // function inside that object

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
  prompt::create();
  living::create();

  enable_commands();

  // name = "";
  // inner_player = nil;

  redirect_input_ob       = nil;
  redirect_input_function = "";

  set_heart_beat(1);

  // if (object_name(previous_object())!=DRIVER) {
  //   destruct_object(this_object());
  // }
}

void init()
{
  history::init();
  alias::init();

  living::init();
}

// Called from the input_to efun
nomask int set_input_to(object obj, string func, varargs int flag, mixed arg) 
{
  if (redirect_input_ob == nil) 
  {
    redirect_input_ob = obj;
    redirect_input_function = func;
    
    // if (flag)
    //   send_message(echo = 0);
  
    return TRUE;
  }

  return FALSE;
}

// Called from the driver
static void open()
{
  login::logon();
}

// Called from the driver
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
    
      redirect_input_ob = nil;
      redirect_input_function = "";

      call_other(tmp_redirect_obj, tmp_redirect_func, str);
  
      if (!redirect_input_ob)
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
      show_prompt();
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



  //   if (!str || strlen(str) == 0) 
  //   {
  //     show_prompt();
  //     return;
  //   }

  //   pieces = explode(str, " ");
  //   current_verb = pieces[0];

  //   if (sizeof(pieces) > 1)
  //     params = implode(pieces[1..], " ");
  //   else
  //     params = "";

  //   // write("Has introducido el comando: " + current_verb + "\n");

  //   switch(current_verb)
  //   {
  //     case "quit":
  //     case "salir":
  //       login::quit();
  //       return;

  //     case "users":
  //     case "who":
  //       tmp::cmd_who(params);
  //       break;

  //     case "compile":
  //     case "update":
  //     case "up":
  //       tmp::cmd_compile(params);
  //       break;

  //     case "test":
  //       tmp::cmd_test(params);
  //       break;

  //     default:
  //       write("¿Perdón?\n");
  //       break;
  //   }

  //   if (!redirect_input_ob) 
  //     show_prompt();
    
  //   return;
  // }
}


void heart_beat()
{
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
                 prompt::stats() + 
                 living::stats();
}





/*
 * NAME:  set_inner_player()
 * DESCRIPTION: set the inner_player object associated with this user
 */
// void set_inner_player(object obj) {
//     if (object_name(previous_object()) == DRIVER) {
//   inner_player = obj;
//     }
//     return;
// }

/*
 * NAME:  query_inner_player()
 * DESCRIPTION: return the inner_player object associated with this user
 */
// object query_inner_player() {
//     return inner_player;
// }


// static void set_name(string str) {
//     name=str;
// }

/* query_name returns the inner_player's name. */

// string query_name() {
//     return name;
// }



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
