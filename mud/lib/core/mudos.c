
/*
 *  Unique object storing global values about the current
 *  operation being performed, providing functions to the
 *  mudlib to emulate MudOS internals.
 */

#include <mud/mudos.h>

inherit hb "/lib/core/heart_beats";

private static object initiator_user, initiator_player, initiator_object;
private static string current_command;
private static string current_verb;
private static string notify_fail_msg;

private static mixed ** call_outs;

void create()
{
  current_verb = "";
  current_command = "";
  notify_fail_msg = "";
  call_outs = ({ });
  hb::create();
}

// TODO security in set_current_verb
string query_current_verb() { return current_verb; }
void set_current_verb(string v) { current_verb = v; }

// TODO security in set_current_command
string query_current_command() { return current_command; }
void set_current_command(string v) { current_command = v; }

// TODO security in set_notify_fail_msg
string query_notify_fail_msg() { return notify_fail_msg; }
void set_notify_fail_msg(string msg) { notify_fail_msg = msg; }

int set_initiator_object(object ob)
{
  if (!mudlib_privileges())
  {
    if (objectp(ob))
      stderr(" *** illegal set_initiator_object with <" + object_name(ob) + ">\n");
    return 0;
  }
  initiator_object = ob;

  // stderr(
  //   " 1 initiator_object: " + to_string(initiator_object) +
  //   " 1 initiator_player: " + to_string(initiator_player) +
  //   " 1 initiator_user: " + to_string(initiator_user)
  //   );

  return 1;
}

int set_initiator_user(object user)
{
  if (!mudlib_privileges())
  {
    if (objectp(user))
      stderr(" *** illegal set_initiator_user with <" + object_name(user) + ">\n");
    return 0;
  }

  initiator_user = user;

  if (!user || !(initiator_player = user->query_player_ob()))
    initiator_player = nil;

  // stderr(
  //   " 2 initiator_object: " + to_string(initiator_object) +
  //   " 2 initiator_player: " + to_string(initiator_player) +
  //   " 2 initiator_user: " + to_string(initiator_user)
  //   );

  return 1;
}

int set_initiator_player(object player)
{
  if (!mudlib_privileges())
  {
    if (objectp(player))
      stderr(" *** illegal set_initiator_player with <" + object_name(player) + ">\n");
    return 0;
  }

  initiator_player = player;
  if (!player || !(initiator_user = player->user()))
    initiator_user = nil;

  // stderr(
  //   " 3 initiator_object: " + to_string(initiator_object) +
  //   " 3 initiator_player: " + to_string(initiator_player) +
  //   " 3 initiator_user: " + to_string(initiator_user)
  //   );

  return 1;
}

object this_player(varargs int i)
{
  if (i && (i == 1))
  {
    if (initiator_player)
      return initiator_player;
    return initiator_user;
  }

  return initiator_object;
  // return initiator_player;
}

object this_user()
{
  if (initiator_user)
    return initiator_user;

  return nil;
}

// call init in ob2 with ob1 as this_player()
void do_init(object ob1, object ob2)
{
  if (!mudlib_privileges())
  {
    stderr(" *** illegal do_init with <" + object_name(ob1) + "> and <" + object_name(ob2) + ">\n");
    return;
  }

  initiator_player = ob1;
  if (!(initiator_user = ob1->user()))
    initiator_user = nil;

  // stderr(
  //   " 4 this_object() es : " + to_string(this_object()) +
  //   " 4 initiator_object: " + to_string(initiator_object) +
  //   " 4 initiator_player: " + to_string(initiator_player) +
  //   " 4 initiator_user: " + to_string(initiator_user)
  //   );

  catch(call_other(ob2, "__call_other", "init"));
}

static void _purge_call_out_stack()
{
  int i, time, size;

  size = sizeof(call_outs);

  if (!size)
    return;

  time = time();

  for (i = 0; i < size; i++)
  {
    if (call_outs[i][CALL_OUT_STACK_TIME] <= time)
    {
      if (i+1 > size)
        call_outs = ({ });
      else
        call_outs = call_outs[i+1..];

      i--;
      size--;
    }
    else
      break;
  }
}

int query_call_out_stack_size()
{
  _purge_call_out_stack();
  return sizeof(call_outs);
}
mixed ** query_call_out_stack()
{
  _purge_call_out_stack();
  return call_outs;
}

int _call_out(object ob, int handle, string func, int delay, varargs mixed args...)
{
  int i, time;

  if (delay < 0)
    delay = 0;

  // every time we modify or check the list, we purge the pending call_out
  _purge_call_out_stack();

  time = time() + delay;

  // sorted so the purge_call_out_stack call is faster
  for (i = sizeof(call_outs) - 1; i >= 0; i--)
  {
    if (time >= call_outs[i][CALL_OUT_STACK_TIME])
      break;
  }

  // stderr("CALL_OUT: " + to_string(call_outs) + "\n");

  call_outs = insert(call_outs, ({ ob, handle, func, time, args, this_player(), this_user() }), i+1);
}

int _remove_call_out(object ob, int handle)
{
  int i, ret;

  // every time we modify or check the list, we purge the pending call_out
  _purge_call_out_stack();

  for (i = sizeof(call_outs) - 1; i >= 0; i--)
  {
    if ((call_outs[i][CALL_OUT_STACK_OB] == ob) &&
        (call_outs[i][CALL_OUT_STACK_HANDLE] == handle))
    {
      // stderr("REMOVE_CALL_OUT: " + to_string(call_outs) + " " +
      //   object_name(ob) + " " +
      //   handle + "\n");

      // time left to the call_out
      ret = call_outs[i][CALL_OUT_STACK_TIME] - time();

      // remove the pending call_out from the list
      call_outs = delete(call_outs, i, 1);

      return ret;
    }
  }

  return -1;
}

int _find_call_out(object ob, string func)
{
  int i;

  // every time we modify or check the list, we purge the pending call_out
  _purge_call_out_stack();

  for (i = sizeof(call_outs) - 1; i >= 0; i--)
  {
    if ((call_outs[i][CALL_OUT_STACK_OB] == ob) &&
        (call_outs[i][CALL_OUT_STACK_FUNC] == func))
    {
      return call_outs[i][CALL_OUT_STACK_TIME] - time();
    }
  }

  return -1;
}

mixed * _call_out_info()
{
  mixed * ret;
  int i, time;

  ret = ({ });
  time = time();

  // every time we modify or check the list, we purge the pending call_out
  _purge_call_out_stack();

  // An array is returned,
  // where every item in the array consists 4 elements: the object,
  // the function, the delay to go, and the optional argument.
  for (i = 0; i < sizeof(call_outs); i++)
  {
    ret += ({
              ({
                call_outs[i][CALL_OUT_STACK_OB],
                call_outs[i][CALL_OUT_STACK_FUNC],
                call_outs[i][CALL_OUT_STACK_TIME] - time,
              })
              +
              (sizeof(call_outs[i][CALL_OUT_STACK_ARG]) ?
                ({ call_outs[i][CALL_OUT_STACK_ARG] }) :
                ({ }))
            });
  }

  return ret;
}
