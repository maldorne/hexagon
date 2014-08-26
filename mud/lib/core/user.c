
inherit "/lib/core/user/login";

#include <user.h>
#include <kernel.h>

static string name;         // user name != inner_player name ?
static object inner_player; // The inner_player object attached to this user

static object redirect_input_ob;       // object that will catch input and
static string redirect_input_function; // function inside that object


// Function prototypes
// static void open();
// static void close(varargs mixed dest);
// static void receive_message(string str);
// nomask int set_input_to(object obj, string func, varargs int flag, mixed arg) 
void write_prompt();


void create() 
{
  ::create();

  name = "";
  inner_player = nil;

  redirect_input_ob       = nil;
  redirect_input_function = "";

  // if (object_name(previous_object())!=DRIVER) {
  //   destruct_object(this_object());
  // }
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
  ::logon();
}

// Called from the driver
static void close(mixed arg)
{
  ::disconnect(FALSE);
}

void write_prompt() 
{
  send_message("> ");
}

void do_efun_write(string str)
{
  if (str == nil)
    return;

  send_message(str);
}

// Called from the driver
static void receive_message(string str) 
{
  string tmp_redirect_func;
  object tmp_redirect_obj;
  int i;

  rlimits (MAX_USER_DEPTH; MAX_USER_TICKS) 
  {
    timestamp = time();

    if (redirect_input_ob) 
    {
      tmp_redirect_obj =  redirect_input_ob;
      tmp_redirect_func = redirect_input_function;
    
      redirect_input_ob = nil;
      redirect_input_function = "";

      call_other(tmp_redirect_obj, tmp_redirect_func, str);
  
      if (!redirect_input_ob)
        write_prompt();
      
      return;
    }

    if (!str || strlen(str) == 0) 
    {
      write_prompt();
      return;
    }

    // inner_player->command(str);
    write("Has introducido el comando: " + str + "\n");

    // TMP
    if (str == "quit")
    {
      ::quit();
      return;
    }
    else if (str == "users")
    {
      object * users;
      users = find_object("/lib/handlers/users")->query_users();

      write("Hay conectados: " + sizeof(users) + " usuarios.\n");
    }

    if (!redirect_input_ob) 
      write_prompt();
    
    return;
  }
}







// static int echo;     is input echoing turned on 
// static int editing;   /* are we editing? */


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
