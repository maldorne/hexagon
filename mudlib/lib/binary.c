
#include <user/login.h>
#include <mud/secure.h>
#include <net/binary.h>
#include <kernel.h>

inherit obj "/lib/core/object.c";

static int timestamp;     // last input time
int start_time;           // time of first connection

void create()
{
  obj::create();

  seteuid(NETWORK_EUID);

  timestamp      = 0;
  start_time     = time();
}

// void init()
// {
//   // main inherit last
//   obj::init();
// }

// do not allow movement of the binary object
int move(mixed dest, varargs mixed messin, mixed messout)
{
  return obj::move(LOGIN_LOUNGE, messin, messout);
}

nomask int query_link() { return 0; }
nomask int query_player() { return 0; }
nomask int query_user() { return 0; }
nomask object user() { return this_object(); }
// total time connected
nomask int query_timestamp() { return timestamp; }

void dest_me()
{
  // remove the user from the user handler
  if (clonep(this_object()))
    find_object(BINARY_HANDLER)->remove_connection(this_object());

  obj::dest_me();
}

// called from the driver
static void open()
{
  mixed * old_context;

  timestamp = time();

  stderr(" ~~~ connection::open()\n");

  // preserve current context
  old_context = MUDOS->query_execution_context();;

  MUDOS->set_current_verb("");
  MUDOS->set_current_command("");
  MUDOS->set_notify_fail_msg("");
  MUDOS->set_initiator_user(this_object());
  MUDOS->set_initiator_object(this_object());

  // *********************************************
  // here do something with the stablished 
  // connection (login with users)
  // *********************************************

  // restore the context when the connection happened
  MUDOS->restore_execution_context(old_context);

  stderr(" ~~~ end connection::open()\n");
}

// called from the driver
static void close(int flag) 
{
  stderr(" ~~~ connection::close(" + flag + ")\n");
}

nomask void save_me() {}

nomask int restore_me(varargs string whatever)
{
  return 0;
}

void send_message(string str)
{
  if (str == nil)
    return;

  ::send_message(str);
}

// called from the driver

// When the output buffer has emptied, message_done() will be called in
// the user object.
// void message_done()
// {
// }

// the skeleton of this function started taking the melville received_message
// as the general idea, but I think it will change a lot with time
static void receive_message(string str)
{
  mixed * old_context;

  rlimits (MAX_BINARY_DEPTH; MAX_BINARY_TICKS)
  {
    timestamp = time();

    str = trim(str);

    stderr(" ~~~ connection::receive_message()\n");

    // the new line has content, so we have a new this_player()
    // preserve current context
    old_context = MUDOS->query_execution_context();

    MUDOS->set_current_verb("");
    MUDOS->set_current_command("");
    MUDOS->set_notify_fail_msg("");
    MUDOS->set_initiator_user(this_object());
    MUDOS->set_initiator_object(this_object());

    // *********************************************
    // here do something with the message received
    // *********************************************

    // restore the context when the message was received
    MUDOS->restore_execution_context(old_context);

    stderr(" ~~~ end connection::receive_message()\n");
  } // rlimits
}

int query_start_time() { return start_time; }

// query_idle - determine how many seconds an interactive player has been idle
// int query_idle( object ob );
// Query how many seconds a player object (ob) has been idling.

nomask int query_idle()
{
  return time() - timestamp;
}

mixed * stats()
{
  return ({
    ({ "Timestamp (nosave)", timestamp, }),
          }) + obj::stats();
}
